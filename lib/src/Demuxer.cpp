#include <cassert>
#include <thread>
#include <iostream>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

#include "checked_calls.hpp"

#include <gpc/_av/config.hpp>
#include <gpc/_av/VideoDecoder.hpp>
#include <gpc/_av/VideoStream.hpp>

#include <gpc/_av/Demuxer.hpp>

GPC_AV_NAMESPACE_START

using namespace std;

// PIMPL DECLARATION ------------------------------------------------

struct Demuxer::Impl {

    //static const size_t INBUF_SIZE = 4096;

    enum ReaderCommand { NOP = 0, SUSPEND, RESUME, TERMINATE };
    enum ReaderState { UNDEFINED = 0, WAITING_FOR_DATA, PROCESSING_DATA, DELIVERING_DATA, SUSPENDED, TERMINATED };

    AVFormatContext                *format_context;
    AVStream                       *video_stream;
    std::unique_ptr<VideoDecoder>   video_decoder;
    std::unique_ptr<std::thread>    reader_thread;
    mutex                           reader_mutex;
    condition_variable              command_posted;     // reader thread waits for this after suspending
    condition_variable              command_executed;   // main thread waits for this after setting a command
    ReaderCommand                   reader_command;
    ReaderState                     reader_state;

    bool                            reader_aborted;
    string                          reader_error;

    Impl();
    ~Impl();

    void open(const std::string &url);
    void start();
    void stop();
    auto make_video_decoder(AVStream *) -> VideoDecoder *;
    auto find_best_video_stream() -> AVStream*;
    auto is_suspended() -> bool;
    void suspend();
    void resume();

    void reader_loop();
};

// MODULE INITIALIZER ----------------------------------------------

static struct ModInit {
    ModInit() {
        av_register_all();
    }
} mod_init;

// LIFECYCLE --------------------------------------------------------

Demuxer::Demuxer(): p(new Impl())
{
    std::cerr << "Demuxer ctor called" << std::endl;
}

Demuxer::~Demuxer() 
{ 
    std::cerr << "Demuxer dtor called" << std::endl;
}

Demuxer::Demuxer(Demuxer&& from)
{
    p.swap(from.p);
}

Demuxer& Demuxer::operator = (Demuxer&& from)
{
    p.swap(from.p);

    return *this;
}

auto Demuxer::create(const std::string &url) -> Demuxer*
{
    Demuxer *demux = new Demuxer();

    demux->p->open(url);

    return demux;
}

// PUBLIC METHODS ----------------------------------------------

void Demuxer::open(const std::string &url)
{
    p->open(url);
}

/*
auto Demuxer::find_best_video_stream() -> VideoStream
{
    return VideoStream(p->find_best_video_stream());
}
*/

void Demuxer::start()
{
    p->start();
}

void Demuxer::stop()
{
    p->stop();
}

auto Demuxer::video_stream() -> VideoStream
{
    assert(p->video_stream);

    return VideoStream(p->video_stream);
}

auto Demuxer::video_decoder() -> VideoDecoder&
{
    if (!p->video_decoder)
    {
        (void) p->make_video_decoder(p->find_best_video_stream());
    }

    return * p->video_decoder.get();
}

auto Demuxer::is_suspended() -> bool
{
    return p->is_suspended();
}

void Demuxer::suspend()
{
    p->suspend();
}

void Demuxer::resume()
{
    p->resume();
}

// PRIVATE IMPLEMENTATION (PIMPL) ----------------------------------

Demuxer::Impl::Impl():
    format_context(nullptr),
    video_stream(nullptr),
    reader_command(NOP)
{
}

Demuxer::Impl::~Impl()
{
	std::cout << "Demuxer::Impl dtor called" << std::endl;

    stop();
}

void Demuxer::Impl::open(const std::string &url)
{
    assert(!format_context);

    _av(avformat_open_input, &format_context, url.c_str(), nullptr, nullptr); // TODO: support options in last parameter

}

void Demuxer::Impl::start()
{
    reader_thread.reset(new std::thread(std::bind(&Impl::reader_loop, this)));
}

void Demuxer::Impl::stop()
{
    if (reader_state != TERMINATED)
    {
        unique_lock<mutex> lk(reader_mutex);
        reader_command = TERMINATE;
        command_posted.notify_one();
        lk.unlock();
        reader_thread->join();
        reader_thread.reset();
    }
}

auto Demuxer::Impl::find_best_video_stream() -> AVStream*
{
    int st_idx = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (st_idx < 0) throw Error(st_idx, "Could not find video stream in source");

    return format_context->streams[st_idx];
}

auto Demuxer::Impl::make_video_decoder(AVStream *stream) -> VideoDecoder *
{
    assert(!video_stream);
    assert(!video_decoder);

    video_stream = stream;
    video_decoder.reset(VideoDecoder::create_from_stream(VideoStream(stream)));

    return video_decoder.get(); // for convenience only
}

auto Demuxer::Impl::is_suspended() -> bool
{
    // No need to lock the access, as the reader thread cannot exit suspended state on its own

    return reader_state == SUSPENDED;
}

/*  TODO: this implementation may suspend the caller until the currently active
        reading operation (in the reader thread) is done or times out. It may
        be better to return immediately and put the player into a "command queued"
        state in which it will refuse further commands.
 */
void Demuxer::Impl::suspend()
{
    cout << "Demuxer: suspending..." << endl;

    unique_lock<mutex> lk(reader_mutex);
    reader_command = SUSPEND;
    command_posted.notify_one();
    command_executed.wait(lk, [this]() { return reader_state == SUSPENDED || reader_state == TERMINATED; });

    cout << "Demuxer: now suspended (or terminating)." << endl;
}

void Demuxer::Impl::resume()
{
    // TODO: support multiple streams with multiple consumers, i.e. only resume when number of
    // calls to resume() matches number of congested consumers

    cout << "Demuxer: resuming..." << endl;

    unique_lock<mutex> lk(reader_mutex);

    assert(reader_state == SUSPENDED);

    reader_command = RESUME;
    command_posted.notify_one();
    command_executed.wait(lk, [this]() { return reader_state != SUSPENDED; });

    cout << "Demuxer: resumed." << endl;
}

void Demuxer::Impl::reader_loop()
{
    try {
        
        // TODO: wait for command_posted before starting ?
        auto &vid_dec = *video_decoder; // TODO: support the passing of a specific stream

        //uint8_t buffer[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
        //memset(buffer + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);

        AVPacket packet;
        av_init_packet(&packet);

        while (reader_command != TERMINATE)
        {
            reader_state = WAITING_FOR_DATA;    // TODO: define and use set_state() ?

            _av(av_read_frame, format_context, &packet);

            reader_state = PROCESSING_DATA; // TODO: define and use set_state() ?

            if (vid_dec.decode_packet(&packet)) // TODO: call impl directly ?
            {
                if (!vid_dec.all_sinks_ready())
                {
                    unique_lock<mutex> lk(reader_mutex);

                    cerr << "Demuxer (reader thread): entering SUSPENDED state" << endl; // TODO: use log

                    reader_state = SUSPENDED; // TODO: notifications ?
                    command_posted.wait(lk, [this]() { return reader_command == RESUME || reader_command == TERMINATE; });

                    if (reader_command == TERMINATE) break;

                    //lk.unlock();
                    reader_state = DELIVERING_DATA; // TODO: notification ?
                    reader_command = NOP;
                    command_executed.notify_one();

                    cerr << "Demuxer (reader thread): resuming (or terminating)" << endl;
                }
                
                if (reader_command == TERMINATE) break;

                vid_dec.deliver_frame();
            }
        }

        reader_state = TERMINATED; // TODO: notification ?
    }
    catch (const exception &e) 
    {
        reader_aborted = true;
        cerr << "Demuxer (reader thread): error while reading: " << e.what() << endl;
        reader_error = e.what();
    }
}

GPC_AV_NAMESPACE_END


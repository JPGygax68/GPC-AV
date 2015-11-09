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
    enum ReaderState { UNDEFINED = 0, WAITING_FOR_DATA, PROCESSING_DATA, SUSPENDED, TERMINATING };

    AVFormatContext                *format_context;
    std::unique_ptr<VideoDecoder>   video_decoder;
    std::unique_ptr<std::thread>    reader_thread;
    //bool                            terminate;
    ReaderCommand                   reader_command;
    ReaderState                     reader_state;
    mutex                           reader_mutex;
    condition_variable              reader_condvar;
    bool                            reader_aborted;
    string                          reader_error;

    Impl();
    ~Impl();

    void open(const std::string &url);
    void start();
    void stop();
    auto get_video_decoder() -> VideoDecoder&;
    auto find_best_video_stream() -> VideoStream;
    void suspend();
    void resume();

    void reader_loop();
};

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

void Demuxer::open(const std::string &url)
{
    p->open(url);
}

void Demuxer::start()
{
    p->start();
}

void Demuxer::stop()
{
    p->stop();
}

auto Demuxer::video_decoder() -> VideoDecoder&
{
    return p->get_video_decoder();
}

// MODULE INITIALIZER ----------------------------------------------

static struct ModInit {
    ModInit() {
        av_register_all();
    }
} mod_init;

void Demuxer::suspend()
{
    p->suspend();
}

void Demuxer::resume()
{
    p->resume();
}

auto Demuxer::find_best_video_stream() -> VideoStream
{
    return p->find_best_video_stream();
}

// PRIVATE IMPLEMENTATION (PIMPL) ----------------------------------

Demuxer::Impl::Impl():
    format_context(nullptr),
    reader_command(NOP)
{
}

Demuxer::Impl::~Impl()
{
	std::cout << "Demuxer::Impl dtor called" << std::endl;

    reader_command = TERMINATE;
    if (reader_thread) reader_thread->join();
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
    reader_command = TERMINATE;
    reader_thread->join();
    reader_thread.reset();
}

// TODO: what if there is no video stream ? 
//  -> perhaps it's better to return a pointer

auto Demuxer::Impl::get_video_decoder() -> VideoDecoder&
{
    if (!video_decoder)
    {
        VideoStream stream = find_best_video_stream();

        video_decoder.reset(VideoDecoder::create_from_stream(stream));
    }

    return *video_decoder;
}

auto Demuxer::Impl::find_best_video_stream() -> VideoStream
{
    int st_idx = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (st_idx < 0) throw Error(st_idx, "Could not find video stream in source");

    auto stream = format_context->streams[st_idx];

    return VideoStream(stream);
}

/*  TODO: this implementation may suspend the caller until the currently active
        reading operation (in the reader thread) is done or times out. It may
        be better to return immediately and put the player into a "command queued"
        state in which it will refuse further commands.
 */
void Demuxer::Impl::suspend()
{
    unique_lock<mutex> lk(reader_mutex);

    cout << "Demuxer: suspending..." << endl;

    reader_command = SUSPEND;
    reader_condvar.wait(lk, [this]() { return reader_state == SUSPENDED; });

    cout << "Demuxer: now suspended." << endl;
}

void Demuxer::Impl::resume()
{
    assert(reader_state == SUSPENDED);

    cout << "Demuxer: resuming..." << endl;

    reader_command = RESUME;
    reader_condvar.notify_one();

    unique_lock<mutex> lk(reader_mutex);
    reader_condvar.wait(lk, [this]() { return reader_state == WAITING_FOR_DATA; });

    cout << "Demuxer: accepting data again." << endl;
}

void Demuxer::Impl::reader_loop()
{
    try {

        auto &vid_dec = get_video_decoder();
        vid_dec.initialize();

        //uint8_t buffer[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
        //memset(buffer + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);

        AVPacket packet;
        av_init_packet(&packet);

        while (reader_command != TERMINATE)
        {
            if (reader_command == SUSPEND)
            {
                reader_state = SUSPENDED; // TODO: define and use set_state() ?
                reader_command = NOP;
                unique_lock<mutex> lk(reader_mutex);
                reader_condvar.wait(lk, [this]() { return reader_command == RESUME || reader_command == TERMINATE; });
            }
            else 
            {
                reader_state = WAITING_FOR_DATA; // TODO: define and use set_state() ?
                _av(av_read_frame, format_context, &packet);

                reader_state = PROCESSING_DATA; // TODO: define and use set_state() ?
                vid_dec.decode_packet(&packet); // TODO: replace with "queue_packet()"  (decoupled) ?
                                                // TODO: call impl directly (get FFmpeg structs out of interfaces) ?
            }
        }

        reader_state = TERMINATING; // TODO: define and use set_state() ?
        vid_dec.cleanup();
        reader_state = UNDEFINED; // TODO: define and use set_state() ?
    }
    catch (const exception &e) 
    {
        reader_aborted = true;
        cerr << "Demuxer error while reading: " << e.what() << endl;
        reader_error = e.what();
    }
}

GPC_AV_NAMESPACE_END


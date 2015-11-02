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

#include <gpc/_av/Demuxer.hpp>

GPC_AV_NAMESPACE_START

using namespace std;

// PIMPL DECLARATION ------------------------------------------------

struct Demuxer::Impl {

    static const size_t INBUF_SIZE = 4096;

    AVFormatContext                *format_context;
    std::unique_ptr<VideoDecoder>   video_decoder;
    std::unique_ptr<std::thread>    reader_thread;
    bool                            reader_aborted;
    string                          reader_error;
    bool                            terminate;

    Impl();
    ~Impl();

    void open(const std::string &url);
    void start();
    void stop();
    auto get_video_decoder() -> VideoDecoder&;

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

// PRIVATE IMPLEMENTATION (PIMPL) ----------------------------------

Demuxer::Impl::Impl():
    format_context(nullptr),
    terminate(false)
{
}

Demuxer::Impl::~Impl()
{
	std::cout << "Demuxer::Impl dtor called" << std::endl;

    terminate = true;
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
    terminate = true;
    reader_thread->join();
    reader_thread.reset();
}

// TODO: what if there is no video stream ? 
//  -> perhaps it's better to return a pointer

auto Demuxer::Impl::get_video_decoder() -> VideoDecoder&
{
    if (!video_decoder)
    {
        int st_idx = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
        if (st_idx < 0) throw Error(st_idx, "Could not find video stream in source");

        auto stream = format_context->streams[st_idx];

        video_decoder.reset(VideoDecoder::createFromStream(stream));
    }

    return *video_decoder;
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

        while (!terminate)
        {
            _av(av_read_frame, format_context, &packet);

            vid_dec.decode_packet(&packet);
        }

        vid_dec.cleanup();
    }
    catch (const exception &e) {
        reader_aborted = true;
        cerr << "Demuxer error while reading: " << e.what() << endl;
        reader_error = e.what();
    }
}

GPC_AV_NAMESPACE_END


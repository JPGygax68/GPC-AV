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

// PIMPL DECLARATION ------------------------------------------------

struct Demuxer::Private {

    AVFormatContext                *format_context;
    std::unique_ptr<VideoDecoder>   video_decoder;
    std::thread                     reader_thread;
    bool                            terminate;

    Private();
    ~Private();

    void open(const std::string &url);
    auto get_video_decoder() -> VideoDecoder&;

    void reader_loop();
};

// LIFECYCLE --------------------------------------------------------

Demuxer::Demuxer(): p(new Private())
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

Demuxer::Private::Private():
    format_context(nullptr),
    terminate(false),
    reader_thread(std::bind(&Private::reader_loop, this))
{
}

Demuxer::Private::~Private()
{
	std::cout << "Demuxer::Private dtor called" << std::endl;

    terminate = true;
    reader_thread.join();
}

void Demuxer::Private::open(const std::string &url)
{
    assert(!format_context);

    _av(avformat_open_input, &format_context, url.c_str(), nullptr, nullptr); // TODO: support options in last parameter
}

auto Demuxer::Private::get_video_decoder() -> VideoDecoder&
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

void Demuxer::Private::reader_loop()
{
	while (!terminate)
	{
        // Repeat until queue is full
        while (!terminate)
        {
            auto queue_size = [this]() {
                return 0;
            }();
        }
	}
}

GPC_AV_NAMESPACE_END

#include <cassert>
#include <thread>
#include <iostream>

extern "C" {
#include "libavformat/avformat.h"
}

#include <gpc/_av/config.hpp>

#include "checked_calls.hpp"

#include <gpc/_av/Demuxer.hpp>

GPC_AV_NAMESPACE_START

// PIMPL DECLARATION ------------------------------------------------

struct Demuxer::Private {

    AVFormatContext *format_context;
    std::thread      reader_thread;

    Private();
    ~Private();

    void _open(const std::string &url);

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

Demuxer::Demuxer(Demuxer&&) = default;

//Demuxer& Demuxer::operator = (Demuxer&&) = default;

Demuxer& Demuxer::operator = (Demuxer&& from)
{
	p.swap(from.p);

	return *this;
}

auto Demuxer::create(const std::string &url) -> Demuxer*
{
	Demuxer *demux = new Demuxer();

    demux->p->_open(url);

	return demux;
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
    reader_thread(std::bind(&Private::reader_loop, this))
{
}

Demuxer::Private::~Private()
{
	std::cout << "Demuxer::Private dtor called" << std::endl;

    reader_thread.join();
}

void Demuxer::Private::_open(const std::string &url)
{
    assert(!format_context);

    _av(avformat_open_input, &format_context, url.c_str(), nullptr, nullptr); // TODO: support options in last parameter
}

void Demuxer::Private::reader_loop()
{
}

GPC_AV_NAMESPACE_END

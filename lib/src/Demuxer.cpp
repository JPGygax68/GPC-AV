#include <cassert>

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

    Private();
    ~Private();

    void _open(const std::string &url);
};

// LIFECYCLE --------------------------------------------------------

Demuxer::Demuxer(): p(new Private()) {}

Demuxer::~Demuxer() { delete p; }

auto Demuxer::create(const std::string &url) -> Demuxer&&
{
    Demuxer demux;

    demux.p->_open(url);

    return std::move(demux);
}

// MODULE INITIALIZER ----------------------------------------------

static struct ModInit {
    ModInit() {
        av_register_all();
    }
} mod_init;

// PRIVATE IMPLEMENTATION (PIMPL) ----------------------------------

Demuxer::Private::Private():
    format_context(nullptr)
{
}

Demuxer::Private::~Private()
{
}

void Demuxer::Private::_open(const std::string &url)
{
    assert(!format_context);

    _av(avformat_open_input, &format_context, url.c_str(), nullptr, nullptr); // TODO: support options in last parameter
}

GPC_AV_NAMESPACE_END

#include <string>
using std::string;
#include <algorithm> // copy

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

#include "checked_calls.hpp"

#include <gpc/_av/Muxer.hpp>

GPC_AV_NAMESPACE_START

// IMPLEMENTATION STRUCT DECLARATION (PIMPL) ------------------------

struct Muxer::Impl {

    void open(const std::string &url);
    void close();

    AVFormatContext    *format_context;
};

// PUBLIC INTERFACE IMPLEMENTATION ----------------------------------

Muxer::Muxer():
    _p(new Impl())
{}

Muxer::~Muxer()
{}

void Muxer::open(const std::string & url)
{
    p()->open(url);
}

void Muxer::close()
{
    p()->close();
}

// INTERNAL IMPLEMENTATION (PIMPL) ----------------------------------

void Muxer::Impl::open(const std::string &url)
{
    using std::copy;
    using std::begin;

    // TODO: support non-url filenames
    string protocol(begin(url), begin(url) + url.find(':'));
    auto output_format = protocol.empty() ? nullptr :_av(av_guess_format, protocol.c_str(), nullptr, nullptr);

    _av(avformat_alloc_output_context2, &format_context, output_format, nullptr, url.c_str());

    /* open the output file, if needed */ 
    if (!(output_format->flags & AVFMT_NOFILE))
    {
        _av(avio_open, &format_context->pb, url.c_str(), AVIO_FLAG_WRITE);
    }
}

void Muxer::Impl::close()
{
}

GPC_AV_NAMESPACE_END

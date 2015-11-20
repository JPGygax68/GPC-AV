extern "C" {
#include "libavformat/avformat.h"
}

#include <gpc/_av/Stream.hpp>

GPC_AV_NAMESPACE_START

// IMPLEMENTATION ---------------------------------------------------

Stream::Stream(AVStream *stream_) : stream(stream_) {}

auto Stream::index() const -> int
{
    return stream->index;
}

auto Stream::media_type() const -> MediaType
{
    return static_cast<MediaType>(stream->codec->codec_type);
}

auto Stream::time_base(const timebase_t &tb) -> Stream&
{
    stream->time_base.num = tb.numerator();
    stream->time_base.den = tb.denominator();

    return *this;
}

auto Stream::time_base() const -> timebase_t
{
    //return Rational(stream->codec->time_base.num, stream->codec->time_base.den);
    return { stream->time_base.num, stream->time_base.den };
}

GPC_AV_NAMESPACE_END

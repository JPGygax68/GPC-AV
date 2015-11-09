extern "C" {
#include "libavformat/avformat.h"
}

#include <gpc/_av/Stream.hpp>

GPC_AV_NAMESPACE_START

// IMPLEMENTATION ---------------------------------------------------

Stream::Stream(AVStream *stream_) : stream(stream_) {}

auto Stream::media_type() const -> MediaType
{
    return static_cast<MediaType>(stream->codec->codec_type);
}

auto Stream::time_base() const -> Rational
{
    return Rational(stream->time_base.num, stream->time_base.den);
}

GPC_AV_NAMESPACE_END

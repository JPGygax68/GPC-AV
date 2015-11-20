extern "C" {
#include "libavformat/avformat.h"
}

#include <gpc/_av/VideoStream.hpp>

GPC_AV_NAMESPACE_START

VideoStream::VideoStream(AVStream *stream) : Stream(stream) {}

auto VideoStream::frame_size(const FrameSize &size) -> VideoStream&
{
    stream->codec->width  = size.w;
    stream->codec->height = size.h;

    return *this;
}

GPC_AV_NAMESPACE_END

#pragma once

#include "config.hpp"

#include "Stream.hpp"

GPC_AV_NAMESPACE_START

class VideoStream: public Stream {
public:
    typedef struct { int w, h; } FrameSize;

    auto frame_size(const FrameSize &) -> VideoStream&;

private:
    friend class Demuxer;
    friend class Muxer;
    friend class VideoDecoder;

    explicit VideoStream(AVStream *);
};

GPC_AV_NAMESPACE_END

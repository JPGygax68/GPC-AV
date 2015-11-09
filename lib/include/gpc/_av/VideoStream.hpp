#pragma once

#include "config.hpp"

#include "Stream.hpp"

GPC_AV_NAMESPACE_START

class VideoStream: public Stream {

private:
    friend class Demuxer;
    friend class VideoDecoder;

    explicit VideoStream(AVStream *);
};

GPC_AV_NAMESPACE_END

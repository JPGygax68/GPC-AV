#pragma once

#include <memory>
#include "config.hpp"
#include "Decoder.hpp"

GPC_AV_NAMESPACE_START

class VideoDecoder : public Decoder {
public:
    ~VideoDecoder();
    VideoDecoder(VideoDecoder&&);
    VideoDecoder& operator = (VideoDecoder&&);

private:
    struct Impl;

    VideoDecoder(Impl*);

    friend class Demuxer;
    static auto createFromStream(void *stream) -> VideoDecoder*;

    std::unique_ptr<Impl> p;
};

GPC_AV_NAMESPACE_END

#pragma once

#include "Frame.hpp"

GPC_AV_NAMESPACE_START

class VideoFrame : public Frame<VideoFrame> {
public:
    typedef struct {
        int w, h;
    } Size;

    using Frame<VideoFrame>::Frame;

    auto size() const -> Size;

private:
    friend class VideoDecoder;
};

GPC_AV_NAMESPACE_END


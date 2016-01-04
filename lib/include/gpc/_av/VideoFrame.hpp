#pragma once

#include "Frame.hpp"

GPC_AV_NAMESPACE_START

class VideoFrame : public Frame<VideoFrame> {
public:
    typedef struct {
        int w, h;
    } Size;

    using Frame::Frame;
    
    auto operator = (const VideoFrame &from) { assign(from); return *this; }

    auto format() const -> int;

    auto size() const -> Size;

    auto Y() const -> const uint8_t *;
    auto U() const -> const uint8_t *;
    auto V() const -> const uint8_t *;

private:
    friend class VideoDecoder;
};

GPC_AV_NAMESPACE_END


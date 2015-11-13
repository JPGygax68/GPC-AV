#include <cassert>

extern "C" {
#include "libavutil/frame.h"
}

#include <gpc/_av/VideoFrame.hpp>

GPC_AV_NAMESPACE_START

auto VideoFrame::_format() const -> int
{
    return frame->format;
}

auto VideoFrame::size() const -> Size
{
    return { frame->width, frame->height };
}

auto VideoFrame::Y() const -> const uint8_t *
{
    assert(frame->data[0]);
    return frame->data[0];
}

auto VideoFrame::U() const -> const uint8_t *
{
    assert(frame->data[1]);
    return frame->data[1];
}

auto VideoFrame::V() const -> const uint8_t *
{
    assert(frame->data[2]);
    return frame->data[2];
}

GPC_AV_NAMESPACE_END

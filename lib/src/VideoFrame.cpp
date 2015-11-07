extern "C" {
#include "libavutil/frame.h"
}

#include <gpc/_av/VideoFrame.hpp>

GPC_AV_NAMESPACE_START

auto VideoFrame::size() const -> Size
{
    return { frame->width, frame->height };
}

GPC_AV_NAMESPACE_END

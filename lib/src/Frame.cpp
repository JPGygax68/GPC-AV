#include <cassert>

extern "C" {
#include "libavutil/frame.h"
}

#include "checked_calls.hpp"

#include <gpc/_av/Frame.hpp>

GPC_AV_NAMESPACE_START

Frame::Frame()
{
    frame = _av(av_frame_alloc);
}

Frame::Frame(Frame&& from)
{
    frame = from.frame;
    from.frame = nullptr;
}

Frame::Frame(const Frame& from)
{
    frame = _av(av_frame_clone, from.frame);
}

Frame & Frame::operator = (const Frame &from)
{
    frame = _av(av_frame_clone, from.frame);

    return *this;
}

Frame & Frame::operator = (Frame &&from)
{
    frame = from.frame;
    from.frame = nullptr;

    return *this;
}

Frame::Frame(AVFrame *frame_): 
    //frame(static_cast<AVFrame*>(frame_))
    frame(frame_)
{
}

void Frame::reset()
{
    av_frame_unref(frame);
}

Frame::~Frame()
{
    //av_frame_free(&frame);
    av_frame_unref(frame);
    //assert(!frame);
}

GPC_AV_NAMESPACE_END

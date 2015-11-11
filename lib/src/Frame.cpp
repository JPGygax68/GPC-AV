#include <cassert>
#include <iostream>

extern "C" {
#include "libavutil/frame.h"
}

#include "checked_calls.hpp"

#include <gpc/_av/Frame.hpp>

GPC_AV_NAMESPACE_START

using namespace std;

// TODO: check if actually reference counted: https://ffmpeg.org/doxygen/2.7/structAVFrame.html#details

#ifdef NOT_DEFINED

static unsigned int alloc_count = 0;
static unsigned int clone_count = 0;
static unsigned int free_count = 0;

static void increment_allocs()
{
    if (++alloc_count % 100 == 0) cerr << "av_frame_alloc() count = " << alloc_count << endl;
}

static void increment_clones()
{
    if (++clone_count % 100 == 0) cerr << "av_frame_clone() count = " << clone_count << endl;
}

static void increment_frees()
{
    if (++free_count % 100 == 0) cerr << "frame free count = " << free_count << endl;
}

#endif

FrameBase::FrameBase()
{
    frame = _av(av_frame_alloc);
    //increment_allocs();
}

FrameBase::FrameBase(FrameBase&& from)
{
    frame = from.frame;
    from.frame = nullptr;
}

FrameBase::FrameBase(const FrameBase& from)
{
    frame = _av(av_frame_clone, from.frame);
    //increment_clones();
}

void FrameBase::assign(const FrameBase &from)
{
    frame = _av(av_frame_clone, from.frame);
    //increment_clones();
}

void FrameBase::assign(FrameBase &&from)
{
    frame = from.frame;
    from.frame = nullptr;
}

FrameBase::FrameBase(AVFrame *from): 
    frame(_av(av_frame_clone, from))
{
    //increment_clones();
}

FrameBase::~FrameBase()
{
    //av_frame_unref(frame);
    av_frame_free(&frame);
    assert(!frame);
    //increment_frees();
}

auto FrameBase::presentation_timestamp() -> int64_t
{
    return frame->pts;
}

GPC_AV_NAMESPACE_END

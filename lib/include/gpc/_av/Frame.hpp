#pragma once

#include "config.hpp"

struct AVFrame;

GPC_AV_NAMESPACE_START

/*  Wrapper for AVFrame.
    Note that copy assignment and copy constructor create clones
    of the frame, while the move constructor and assignment 
    operator just copy the pointer (and set it to nullptr in the
    original).

    TODO: implement some tracing/debugging about frame references?
 */
class Frame {
public:

    Frame();
    Frame(Frame&&);
    Frame(const Frame& from);
    Frame& operator = (const Frame&);
    Frame& operator = (Frame&&);
    ~Frame();

    void reset();

private:
    friend class VideoDecoder;

    explicit Frame(AVFrame *av_frame);

    AVFrame *frame;
};

GPC_AV_NAMESPACE_END
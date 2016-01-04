#pragma once

#include <chrono>

#include "config.hpp"

struct AVFrame;

GPC_AV_NAMESPACE_START

/** Base class for AVFrame wrappers.

    TODO: implement some tracing/debugging about frame references?
*/
class FrameBase {
public:
    //typedef std::chrono::time_point<std::chrono::microseconds> timepoint_t;

    auto presentation_timestamp() const -> int64_t; // in x/time_base (time_base defined in Stream)

protected:
    FrameBase();
    FrameBase(const FrameBase &);
    FrameBase(FrameBase&&);
    ~FrameBase();
    explicit FrameBase(AVFrame *av_frame);

    void assign(const FrameBase &);
    void assign(FrameBase &&);

    AVFrame *frame;
};

/** Template base class that uses the curiously recurring pattern
    to make AVFrame wrappers type-safe.
    Note that copy assignment and copy constructor create clones
    of the frame, while the move constructor and assignment 
    operator just copy the pointer (and set it to nullptr in the
    original).
 */
template <class Class>
class Frame: public FrameBase {

public:
    Frame() : FrameBase{} {}
    Frame(Class&& from) : FrameBase(from) {}
    Frame(const Class& from) : FrameBase(from) {}
    explicit Frame(AVFrame *av_frame) : FrameBase(av_frame) {}

    //Class& operator = (const Class &from) { FrameBase::assign(from); return *this;  }
    //Class& operator = (Class &&from) { FrameBase::assign(from); return *this;  }
};

GPC_AV_NAMESPACE_END
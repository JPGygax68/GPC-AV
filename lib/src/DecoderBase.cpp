#include <utility>
#include <cassert>

extern "C" {
//#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

#include <gpc/_av/internal/DecoderBase.hpp>
#include <gpc/_av/internal/DecoderBase_Impl.hpp>

GPC_AV_NAMESPACE_START

using namespace std;

// PUBLIC INTERFACE -------------------------------------------------

DecoderBase::DecoderBase() :
    _p(new Impl())
{
}

DecoderBase::DecoderBase(DecoderBase&& from)
{
    _p.swap(from._p);
}

DecoderBase & DecoderBase::operator = (DecoderBase&& from)
{
    _p.swap(from._p);

    return *this;
}

auto DecoderBase::time_base() const -> duration_t
{
    return _p->time_base();
}

auto DecoderBase::add_consumer(Consumer consumer) -> int
{
    return _p->add_consumer(consumer);
}

void DecoderBase::remove_consumer(int token)
{
    _p->remove_consumer(token);
}

void DecoderBase::deliver_frame(const Frame &frame)
{
    _p->deliver_frame(frame);
}

// FRIEND INTERFACES ------------------------------------------------

DecoderBase::DecoderBase(Impl *p_) : 
    _p(p_) 
{
}

// IMPLEMENTATION (PIMPL) -------------------------------------------

DecoderBase::Impl::Impl() = default;

DecoderBase::Impl::Impl(AVCodecContext *context_, AVCodec *codec_) :
    context(context_), codec(codec_)
{}

auto DecoderBase::Impl::time_base() const -> duration_t
{
    assert(context);

    return context->time_base;
}

auto DecoderBase::Impl::add_consumer(Consumer &consumer) -> int
{
    int token = consumers.next_token++;
    consumers.map.insert({ token, consumer });
    return token;
}

void DecoderBase::Impl::remove_consumer(int token)
{
    consumers.map.erase(token);
}

void DecoderBase::Impl::deliver_frame(const Frame &frame)
{
    for (auto &entry : consumers.map) 
    {
        entry.second(frame);
    }
}


GPC_AV_NAMESPACE_END

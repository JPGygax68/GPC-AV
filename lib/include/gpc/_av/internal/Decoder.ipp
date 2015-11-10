#pragma once

#include <gpc/_av/internal/Decoder.hpp>

GPC_AV_NAMESPACE_START

template<class Class, class FrameClass>
Decoder<Class, FrameClass>::Decoder(DecoderBase::Impl *impl):
    DecoderBase(static_cast<Class::Impl*>(impl))
{
}

template <class Class, class FrameClass>
auto Decoder<Class, FrameClass>::add_sink(ISink &sink) -> int
{
    return static_cast<Impl*>(_p.get())->add_sink(sink);
}

template <class Class, class FrameClass>
void Decoder<Class, FrameClass>::remove_sink(int token)
{
    static_cast<Impl*>(_p.get())->remove_sink(token);
}

template<class Class, class FrameClass>
auto Decoder<Class, FrameClass>::all_sinks_ready() -> bool
{
    return static_cast<Impl*>(_p.get())->all_sinks_ready();
}

template<class Class, class FrameClass>
void Decoder<Class, FrameClass>::deliver_frame()
{
    static_cast<Impl*>(_p.get())->deliver_frame();
}

GPC_AV_NAMESPACE_END

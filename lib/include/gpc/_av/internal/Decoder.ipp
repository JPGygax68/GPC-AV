#pragma once

#include <gpc/_av/internal/Decoder.hpp>

GPC_AV_NAMESPACE_START

template<class Class, class FrameClass>
Decoder<Class, FrameClass>::Decoder(DecoderBase::Impl *impl):
    DecoderBase(static_cast<Class::Impl*>(impl))
{
}

template <class Class, class FrameClass>
auto Decoder<Class, FrameClass>::add_consumer(Consumer consumer) -> int
{
    return static_cast<Impl*>(_p.get())->add_consumer(consumer);
}

template <class Class, class FrameClass>
void Decoder<Class, FrameClass>::remove_consumer(int token)
{
    static_cast<Impl*>(_p.get())->remove_consumer(token);
}

GPC_AV_NAMESPACE_END

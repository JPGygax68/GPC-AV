#pragma once

#include <gpc/_av/internal/Decoder.hpp>

GPC_AV_NAMESPACE_START

template <class Class, class FrameClass, typename Impl>
auto Decoder<Class, FrameClass, Impl>::add_consumer(Consumer consumer) -> int
{
    return p()->add_consumer(consumer);
}

template <class Class, class FrameClass, typename Impl>
void Decoder<Class, FrameClass, Impl>::remove_consumer(int token)
{
    p()->remove_consumer(token);
}

GPC_AV_NAMESPACE_END

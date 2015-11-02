#include "gpc/_av/config.hpp"

#include <gpc/_av/internal/DecoderBaseImpl.hpp>

GPC_AV_NAMESPACE_START

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

GPC_AV_NAMESPACE_END

#include <map>

#include <gpc/_av/internal/DecoderBase.hpp>
#include <gpc/_av/internal/DecoderBaseImpl.hpp>

GPC_AV_NAMESPACE_START

using namespace std;

// PUBLIC INTERFACE -------------------------------------------------

DecoderBase::DecoderBase(DecoderBase&& from)
{
    _p = std::move(from._p);
}

DecoderBase & DecoderBase::operator = (DecoderBase&& from)
{
    _p = std::move(from._p);

    return *this;
}

auto DecoderBase::add_consumer(Consumer consumer) -> int
{
    return _p->add_consumer(consumer);
}

void DecoderBase::remove_consumer(int token)
{
    _p->remove_consumer(token);
}

GPC_AV_NAMESPACE_END

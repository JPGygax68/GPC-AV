#pragma once

#include <functional>
#include <memory>

#include <gpc/_av/Rational.hpp>

#include "../config.hpp"

GPC_AV_NAMESPACE_START

class FrameBase;
struct DecoderBase_Impl;

class DecoderBase {
public:

    typedef Rational duration_t;

    auto time_base() const -> duration_t;

protected:

    DecoderBase(DecoderBase&&);
    DecoderBase& operator = (DecoderBase&&);
    explicit DecoderBase(DecoderBase_Impl *p_);

    std::unique_ptr<DecoderBase_Impl> _p;
};

template <class Class, class FrameClass, typename Impl>
class Decoder : public DecoderBase {
public:

    typedef FrameClass FrameClass;
    typedef std::function<void(const FrameClass &)> Consumer;

    /** Add or remove a "consumer", i.e. a callback that will receive
    frames as they are decoded.
    */
    auto add_consumer(Consumer consumer) -> int;

    void remove_consumer(int token);

protected:

    Decoder(Impl *impl) : DecoderBase(impl) {}

    auto p() -> Impl *
    {
        return static_cast<Impl*>(_p.get());
    }
};

GPC_AV_NAMESPACE_END

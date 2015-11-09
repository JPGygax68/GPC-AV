#pragma once

#include <functional>
#include <memory>

#include <gpc/_av/Rational.hpp>

#include "../config.hpp"

GPC_AV_NAMESPACE_START

class FrameBase;
//struct DecoderBase_Impl;

class DecoderBase {
public:

    typedef Rational duration_t;

    auto time_base() const -> duration_t;

protected:
    struct Impl;

    DecoderBase(DecoderBase&&);
    DecoderBase& operator = (DecoderBase&&);
    explicit DecoderBase(Impl *p_);

    std::unique_ptr<Impl> _p;
};

template <class Class, class FrameClass>
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
    struct Impl;

    Decoder(Impl *impl) : DecoderBase(impl) {}
    Decoder(DecoderBase::Impl *);

    //auto p() -> Impl * { return static_cast<Impl*>(_p.get()); }
};

GPC_AV_NAMESPACE_END

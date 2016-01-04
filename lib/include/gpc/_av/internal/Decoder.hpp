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

    class ISink {
    public:
        virtual auto ready() -> bool = 0;
    };

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

    class ISink: public DecoderBase::ISink {
    public:
        virtual void process_frame(const FrameClass &) = 0;
    };

    /** Add a sink (i.e. a frame "consumer").
        TODO: overload that accepts a std::function<>
    */
    auto add_sink(ISink&) -> int;

    void remove_sink(int token);

    auto all_sinks_ready() -> bool;

    void deliver_frame();

protected:
    struct Impl;

    Decoder(Impl *impl) : DecoderBase(impl) {}
    Decoder(DecoderBase::Impl *);

    //auto p() -> Impl * { return static_cast<Impl*>(_p.get()); }
};

GPC_AV_NAMESPACE_END
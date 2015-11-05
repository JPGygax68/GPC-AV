#pragma once

#include <functional>
#include <memory>

#include <gpc/_av/Rational.hpp>

#include "../config.hpp"

GPC_AV_NAMESPACE_START

class Frame;

class DecoderBase {
public:

    typedef Rational<int64_t> duration_t;

    typedef std::function<void(const Frame&)> Consumer;

    DecoderBase();
    DecoderBase(DecoderBase&&);
    DecoderBase& operator = (DecoderBase&&);

    auto time_base() const -> duration_t;

    /** Add or remove a "consumer", i.e. a callback that will receive
        frames as they are decoded.
     */
    auto add_consumer(Consumer) -> int;
    void remove_consumer(int);

protected:

    void deliver_frame(const Frame&);

protected:

    struct Impl;

    explicit DecoderBase(Impl *p_);

    std::unique_ptr<Impl> _p;
};

GPC_AV_NAMESPACE_END

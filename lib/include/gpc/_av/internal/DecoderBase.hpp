#pragma once

#include <functional>
#include <memory>

#include "../config.hpp"

GPC_AV_NAMESPACE_START

class Frame;

class DecoderBase {
public:
    typedef std::function<void(const Frame&)> Consumer;

    DecoderBase(DecoderBase&&);
    DecoderBase& operator = (DecoderBase&&);

    auto add_consumer(Consumer) -> int;
    void remove_consumer(int);

protected:
    struct Impl;

    explicit DecoderBase(Impl *p_) : _p(p_) {}

    Impl *_p;
};

GPC_AV_NAMESPACE_END

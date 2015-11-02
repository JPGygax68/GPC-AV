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

public:
    struct Impl;

protected:
    explicit DecoderBase(Impl *p_) : _p(p_) {}

    Impl *_p;
};

template <typename Derived, typename DerivedImpl>
class Decoder : public DecoderBase {
protected:
    typedef DerivedImpl Impl;

    explicit Decoder(DerivedImpl* impl) : DecoderBase(impl) {}
    ~Decoder() { delete p(); }

    auto p() -> DerivedImpl* { return static_cast<DerivedImpl*>(_p); }
};

GPC_AV_NAMESPACE_END

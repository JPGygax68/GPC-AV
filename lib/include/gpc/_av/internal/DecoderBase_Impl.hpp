#pragma once

#include <map>

#include "../Rational.hpp"

#include "../config.hpp"
#include "../internal/DecoderBase.hpp"

struct AVCodecContext;
struct AVCodec;

GPC_AV_NAMESPACE_START

struct DecoderBase::Impl {

    struct ConsumerList {
        std::map<int, Consumer> map;
        int                     next_token;
    };

    AVCodecContext         *context;
    AVCodec                *codec;
    ConsumerList            consumers;

    Impl();
    Impl(AVCodecContext *, AVCodec *);
    virtual ~Impl() {}

    auto time_base() const -> duration_t;

    auto add_consumer(Consumer &) -> int;
    void remove_consumer(int);

    void deliver_frame(const Frame &);
};

GPC_AV_NAMESPACE_END

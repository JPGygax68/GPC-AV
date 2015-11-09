#pragma once

#include <map>

#include "../Rational.hpp"

#include "../config.hpp"
#include "../internal/Decoder.hpp"

struct AVCodecContext;
struct AVCodec;

GPC_AV_NAMESPACE_START

struct DecoderBase::Impl {

    typedef DecoderBase::duration_t duration_t;

    AVCodecContext         *context;
    AVCodec                *codec;

    Impl();
    Impl(AVCodecContext *, AVCodec *);
    virtual ~Impl() {}

    auto time_base() const -> duration_t;
};

template <class Class, class FrameClass>
struct Decoder<Class, FrameClass>::Impl: public DecoderBase::Impl {

    typedef typename Class::Consumer Consumer;
    typedef typename Class::FrameClass FrameClass;

    struct ConsumerList {
        std::map<int, Consumer> map;
        int                     next_token;
    };

    ConsumerList    consumers;

    using DecoderBase::Impl::Impl;

    auto add_consumer(Consumer consumer) -> int
    {
        int token = consumers.next_token++;
        consumers.map.insert({ token, consumer });
        return token;
    }

    void remove_consumer(int token)
    {
        consumers.map.erase(token);
    }

    void deliver_frame(const FrameClass &frame)
    {
        for (auto &entry : consumers.map)
        {
            entry.second(frame);
        }
    }
};

GPC_AV_NAMESPACE_END

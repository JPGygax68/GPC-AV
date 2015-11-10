#pragma once

#include <map>
#include <algorithm>

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
    AVFrame                *frame;

    Impl();
    Impl(AVCodecContext *, AVCodec *);
    virtual ~Impl() {}

    auto time_base() const -> duration_t;
};

template <class Class, class FrameClass>
struct Decoder<Class, FrameClass>::Impl: public DecoderBase::Impl {

    typedef typename Class::ISink       ISink;

    struct SinkList {
        std::map<int, ISink&>   map;
        int                     next_token;
    };

    SinkList    sinks;

    using DecoderBase::Impl::Impl;

    auto add_sink(ISink &sink) -> int
    {
        int token = sinks.next_token++;
        sinks.map.emplace(token, sink);
        return token;
    }

    void remove_sink(int token)
    {
        sinks.map.erase(token);
    }

    auto all_sinks_ready() const -> bool
    {
        return all_of(begin(sinks.map), end(sinks.map), [](const decltype(sinks.map)::value_type &entry) { return entry.second.ready(); });
    }

    void deliver_frame()
    {
        assert(all_sinks_ready());

        for (auto &entry: sinks.map)
        {
            ISink &sink = entry.second;

            sink.process_frame(FrameClass(frame));
        }
    }
};

GPC_AV_NAMESPACE_END

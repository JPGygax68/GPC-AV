#pragma once

#include <map>

#include "../config.hpp"
#include "../internal/DecoderBase.hpp"

GPC_AV_NAMESPACE_START

struct DecoderBase::Impl {

    struct ConsumerList {
        std::map<int, Consumer> map;
        int                     next_token;
    };

    ConsumerList            consumers;

    virtual ~Impl() {}

    auto add_consumer(Consumer &) -> int;
    void remove_consumer(int);
};

GPC_AV_NAMESPACE_END

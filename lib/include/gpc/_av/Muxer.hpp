#pragma once

#include <string>
#include <memory>

#include "config.hpp"

GPC_AV_NAMESPACE_START

class Muxer {
public:
    Muxer();
    ~Muxer();

    void open(const std::string &url);

    void close();

private:
    struct Impl;

    auto p() const -> Impl * { return _p.get(); }

    std::unique_ptr<Impl>   _p;
};

GPC_AV_NAMESPACE_END

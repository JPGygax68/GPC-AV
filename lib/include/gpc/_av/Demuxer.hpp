#pragma once

#include <string>
#include <memory>

#include "config.hpp"

GPC_AV_NAMESPACE_START

class Demuxer {
public:
    Demuxer();
    Demuxer(const Demuxer&) = delete;
    ~Demuxer();

    static auto create(const std::string &url) -> Demuxer&&;

    void open(const std::string &url);

private:
    struct Private;
    
    Private *p;
};

GPC_AV_NAMESPACE_END

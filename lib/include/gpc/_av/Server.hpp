#pragma once

#include <functional>

#include "config.hpp"

GPC_AV_NAMESPACE_START

class Muxer;

class Server {
public:
    using SessionInitiator = std::function<void(Muxer&)>;

    Server();
    ~Server();

    void listen(const std::string &url, SessionInitiator);

    void close();

private:
    struct Impl;
    Impl *_p;
    Impl *p() { return _p; }
};

GPC_AV_NAMESPACE_END
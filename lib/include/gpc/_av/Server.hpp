#pragma once

#include "config.hpp"

GPC_AV_NAMESPACE_START

class Server {
public:
    Server();
    ~Server();

private:
    struct Impl;
    Impl *_p;
    Impl *p() { return _p; }
};

GPC_AV_NAMESPACE_END
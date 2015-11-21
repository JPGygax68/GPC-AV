#include <gpc/_av/Server.hpp>

GPC_AV_NAMESPACE_START

// IMPLEMENTATION DECLARATION (PIMPL) ------------------------------

struct Server::Impl {
};

// PUBLIC INTERFACE IMPLEMENTATION ----------------------------------

Server::Server():
    _p(new Impl())
{}

Server::~Server()
{}

// IMPLEMENTATION (PIMPL) -------------------------------------------


GPC_AV_NAMESPACE_END

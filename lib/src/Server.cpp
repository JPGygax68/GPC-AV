#include <thread>

extern "C" {
#include "libavformat/avformat.h"
}

#include <gpc/_av/checked_calls.hpp>

#include <gpc/_av/Server.hpp>

GPC_AV_NAMESPACE_START

// IMPLEMENTATION DECLARATION (PIMPL) ------------------------------

struct Server::Impl 
{
    Impl();

    void listen(const std::string &url, SessionInitiator);
};

// PUBLIC INTERFACE IMPLEMENTATION ----------------------------------

Server::Server():
    _p(new Impl())
{
}

Server::~Server()
{}

void Server::listen(const std::string &url, SessionInitiator cb)
{
    p()->listen(url, cb);
}

void Server::close()
{
    // TODO: terminate all sessions
}

// IMPLEMENTATION (PIMPL) -------------------------------------------

Server::Impl::Impl()
{
    av_register_all();
    avformat_network_init(); // TODO: call avformat_network_deinit()
}

void Server::Impl::listen(const std::string &url, SessionInitiator cb)
{
    AVDictionary *options = nullptr;
    AVIOContext *server = nullptr;

    av_log_set_level(AV_LOG_TRACE);

    AV(av_dict_set, &options, "listen", "2", 0);
    int code = AV(avio_open2, &server, url.c_str(), AVIO_FLAG_WRITE, nullptr, &options);

    std::thread listener([this, cb]() {

    });
}

GPC_AV_NAMESPACE_END

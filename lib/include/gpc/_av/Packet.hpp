#pragma once

#include "config.hpp"

struct AVPacket;

GPC_AV_NAMESPACE_START

class Packet {
public:

    Packet();
    ~Packet();

private:
    friend class Demuxer;

    //explicit Packet(AVPacket *pkt);

    AVPacket av_pkt;
};

GPC_AV_NAMESPACE_END

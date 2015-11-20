#include "checked_calls.hpp"

extern "C" {
#include "libavcodec/avcodec.h"
}

#include <gpc/_av/Packet.hpp>

GPC_AV_NAMESPACE_START

Packet::Packet(AVPacket &pkt): av_pkt(pkt)
{
    av_init_packet(&av_pkt);

    av_pkt.stream_index = stream_index;
}

Packet::~Packet()
{
    //av_free_packet(&av_pkt);
    av_packet_unref(&av_pkt);
}

GPC_AV_NAMESPACE_END

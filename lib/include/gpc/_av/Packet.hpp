#pragma once

#include "config.hpp"

struct AVPacket;

GPC_AV_NAMESPACE_START

class Packet {
public:

    typedef std::tuple<uint8_t*, size_t> Data;

    Packet(AVPacket &pkt) {}

    ~Packet();

    auto data(Data) -> Packet&;
    auto data() const -> Data;
    auto pts() const;
    auto pts(int64_t ts) -> Packet&;
    auto duration(int) -> Packet&;
    auto duration() -> int;

private:
    friend class Demuxer;

    //explicit Packet(AVPacket *pkt);

    AVPacket &av_pkt;
};

GPC_AV_NAMESPACE_END

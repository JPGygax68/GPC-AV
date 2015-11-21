#pragma once

#include <string>
#include <memory>

#include "config.hpp"

#include "common.hpp"

#include "CodecIDs.hpp"

GPC_AV_NAMESPACE_START

class VideoStream;

class Muxer {
public:

    Muxer();
    ~Muxer();

    void open(const std::string &url);

    void add_video_stream(CodecID, int width, int height);

    auto video_stream() -> VideoStream;

    void write_header();

    void send_packet(int stream_index, const uint8_t *data, int size, 
        int64_t pts, int64_t dts = NOPTS, int duration = 0, int pos = -1);

    void send_h264_hevc(int stream_index, const uint8_t *data, int size, int64_t pts, int64_t dts = NOPTS, int duration = 0);

    void close();

private:
    struct Impl;

    auto p() const -> Impl * { return _p.get(); }

    std::unique_ptr<Impl>   _p;
};

GPC_AV_NAMESPACE_END

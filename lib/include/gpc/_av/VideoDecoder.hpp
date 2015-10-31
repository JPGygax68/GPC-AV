#pragma once

#include <memory>
#include "config.hpp"
#include "Decoder.hpp"

GPC_AV_NAMESPACE_START

class Frame;

class VideoDecoder : public Decoder {
public:
    ~VideoDecoder();
    VideoDecoder(VideoDecoder&&);
    VideoDecoder& operator = (VideoDecoder&&);

private:
    struct Impl;

    VideoDecoder(Impl*);

private: // Interface with Demuxer class

    friend class Demuxer;
    static auto createFromStream(void *stream) -> VideoDecoder*;

    void initialize();
    void cleanup();

    bool decode_packet(void *packet); // returns true if a new frame was made available

    bool frame_available();

    auto get_next_frame() -> Frame;

    void recycle_frame(Frame&& frame);

    std::unique_ptr<Impl> p;
};

GPC_AV_NAMESPACE_END

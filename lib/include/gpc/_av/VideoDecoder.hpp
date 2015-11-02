#pragma once

#include <memory>
#include "config.hpp"
#include "internal/DecoderBase.hpp"

GPC_AV_NAMESPACE_START

class Frame;

class VideoDecoder : public DecoderBase {
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

    auto p() -> Impl*;

    void initialize();
    void cleanup();

    bool decode_packet(void *packet); // returns true if a new frame was made available

    //bool frame_available();

    //auto get_next_frame() -> Frame;

    //void recycle_frame(Frame&& frame);
};

GPC_AV_NAMESPACE_END

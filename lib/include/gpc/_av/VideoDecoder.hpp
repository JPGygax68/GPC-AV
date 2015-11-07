#pragma once

#include <memory>
#include "config.hpp"
#include "internal/Decoder.hpp"

GPC_AV_NAMESPACE_START

class VideoFrame;
struct VideoDecoder_Impl;

class VideoDecoder : public Decoder<VideoDecoder, VideoFrame, VideoDecoder_Impl> {
public:

    ~VideoDecoder();
    VideoDecoder(VideoDecoder&&);
    VideoDecoder& operator = (VideoDecoder&&);

private:

    VideoDecoder(VideoDecoder_Impl*);

private: // Interface with Demuxer class
    
    friend class Demuxer;
    static auto createFromStream(void *stream) -> VideoDecoder*;

    void initialize();
    void cleanup();

    bool decode_packet(void *packet); // returns true if a new frame was made available
};

GPC_AV_NAMESPACE_END

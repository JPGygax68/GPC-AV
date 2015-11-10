#pragma once

#include <memory>
#include "config.hpp"
#include "internal/Decoder.hpp"

GPC_AV_NAMESPACE_START

class VideoStream;
class VideoFrame;

class VideoDecoder : public Decoder<VideoDecoder, VideoFrame> {
public:
    typedef Decoder<VideoDecoder, VideoFrame> ParentClass;

    struct Impl; // must be public so it can be accessed by template base class

    ~VideoDecoder();
    VideoDecoder(VideoDecoder&&);
    VideoDecoder& operator = (VideoDecoder&&);

protected:

private: // Interface with Demuxer class
    
    friend class Demuxer;
    static auto create_from_stream(const VideoStream &stream) -> VideoDecoder*;

    VideoDecoder(Impl*);

    bool decode_packet(void *packet); // Returns true if consumer(s) can accept more frames

    auto p() -> Impl *;
};

GPC_AV_NAMESPACE_END

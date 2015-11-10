#pragma once

#include <string>
#include <memory>

#include "config.hpp"

GPC_AV_NAMESPACE_START

class VideoDecoder;
class VideoStream;

class Demuxer {
public:

    Demuxer();
	~Demuxer();
	Demuxer(Demuxer&&);
	Demuxer& operator = (Demuxer&&);

    static auto create(const std::string &url) -> Demuxer*;

    void open(const std::string &url);

    void start();
    void stop();

    auto is_suspended() -> bool;

    void suspend();
    void resume();

    auto video_stream() -> VideoStream;

    auto video_decoder() -> VideoDecoder&;

    // FUTURE EXTENSION
    // auto find_best_video_stream() -> VideoStream;

private:
    struct Impl;
    
    std::unique_ptr<Impl> p;
};

GPC_AV_NAMESPACE_END

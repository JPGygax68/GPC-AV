#pragma once

#include <string>
#include <memory>

#include "config.hpp"

GPC_AV_NAMESPACE_START

class VideoDecoder;

class Demuxer {
public:

    Demuxer();
	~Demuxer();
	Demuxer(Demuxer&&);
	Demuxer& operator = (Demuxer&&);

    static auto create(const std::string &url) -> Demuxer*;

    void open(const std::string &url);

    auto video_decoder() -> VideoDecoder&;

private:
    struct Impl;
    
    std::unique_ptr<Impl> p;
};

GPC_AV_NAMESPACE_END

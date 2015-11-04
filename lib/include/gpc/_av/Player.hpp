#pragma once

#include <string>
#include <memory>

#include "internal/DecoderBase.hpp" // TODO: move definition of Consumer into public, separate header ?

#include "config.hpp"

GPC_AV_NAMESPACE_START

class Frame;

class Player {
public:
    typedef DecoderBase::Consumer VideoConsumer;

    static auto create(const std::string &url) -> Player*;

    void add_video_consumer(VideoConsumer consumer);
    // TODO: audio consumers, subtitles ?, etc.

    void open(const std::string &url);

    void play();
    void pause();

private:
    struct Impl;
    std::unique_ptr<Impl> p;

    Player() = default;
};

GPC_AV_NAMESPACE_END


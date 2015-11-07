#pragma once

#include <string>
#include <functional>
#include <memory>

#include "internal/Decoder.hpp" // TODO: move definition of Consumer into public, separate header ?

#include "config.hpp"

GPC_AV_NAMESPACE_START

class VideoFrame;
class AudioFrame;

class Player {
public:

    // TODO: factory method may be obsolete
    static auto create(const std::string &url) -> Player*;

    Player();
    ~Player();

    void open(const std::string &url);

    void play();
    void pause();

    auto peek_newest_video_frame() -> const VideoFrame *;

    auto get_newest_video_frame() -> VideoFrame;

    // TODO: replace with approach not depending on video stream ?
    bool video_frame_available();

    auto current_video_frame() -> const VideoFrame *;

private:
    struct Impl;
    std::unique_ptr<Impl> p;
};

GPC_AV_NAMESPACE_END


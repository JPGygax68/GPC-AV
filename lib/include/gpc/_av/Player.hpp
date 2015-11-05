#pragma once

#include <string>
#include <functional>
#include <memory>

#include "internal/DecoderBase.hpp" // TODO: move definition of Consumer into public, separate header ?

#include "config.hpp"

GPC_AV_NAMESPACE_START

class Frame;

class Player {
public:

    static auto create(const std::string &url) -> Player*;

    ~Player();

    void open(const std::string &url);

    void play();
    void pause();

    auto peek_newest_video_frame() -> const Frame *;

    auto get_newest_video_frame() -> Frame;

    // TODO: replace with approach not depending on video stream ?
    bool video_frame_available();

    auto current_frame() -> const Frame &;

private:
    struct Impl;
    std::unique_ptr<Impl> p;

    Player() = default;
};

GPC_AV_NAMESPACE_END


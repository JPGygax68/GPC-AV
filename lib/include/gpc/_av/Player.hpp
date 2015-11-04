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
    typedef std::function<void(const Frame &)> FramePresenter;

    static auto create(const std::string &url) -> Player*;

    ~Player();

    void set_frame_presenter(FramePresenter presenter);
    // TODO: audio presenter, subtitles ?, etc.

    void open(const std::string &url);

    void play();
    void pause();

private:
    struct Impl;
    std::unique_ptr<Impl> p;

    Player() = default;
};

GPC_AV_NAMESPACE_END


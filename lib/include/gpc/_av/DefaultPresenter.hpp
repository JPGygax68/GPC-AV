#pragma once

#include <memory>
#include <cstdint>

#include "config.hpp"

GPC_AV_NAMESPACE_START

template <typename Impl> class Decoder<Impl>;
class Frame;

class DefaultPresenter {

public:
    struct Rect { unsigned int w, h; };

    typedef void (*PresentCallback)(const Frame *);

    DefaultPresenter(Decoder&, PresentCallback);

    void play();
    void pause();
    void stop();

private:
    struct Impl;

    std::unique_ptr<Impl> p;
};

GPC_AV_NAMESPACE_END

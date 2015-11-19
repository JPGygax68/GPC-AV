#pragma once

#include <memory>
#include <string>

#ifndef DONT_USE_GPC_AV_DEMUXER
#include "_av/Demuxer.hpp"
#endif
#ifndef DONT_USE_GPC_AV_VIDEO_DECODER
#include "_av/VideoDecoder.hpp"
#include "_av/VideoFrame.hpp"
#endif
#ifndef DONT_USE_GPC_AV_VIDEO_PLAYER
#include "_av/Player.hpp"
#endif
#ifndef DONT_USE_GPC_AV_FRAME
// TODO: separate classes for video and audio, which get included automatically as required ?
#include "_av/Frame.hpp"
#endif
#ifndef DONT_USE_GPC_AV_MUXER
#include "_av/Muxer.hpp"
#endif
#ifdef USE_GPC_AV_OPENGL
#include "_av/opengl/YUVPainter.hpp" // TODO: this should become a separate library
#endif

#include "_av/config.hpp"

GPC_AV_NAMESPACE_START
    
// TODO: remove? or move to separate header file ?

class Source {
public:

    Source();
    Source(Source&& orig);
    ~Source();

    void open(const std::string &url);

    /* Open a source by its URL.
        */
    static auto create(const std::string &url) -> Source;

private:

    struct Private;

    std::unique_ptr<Private>    p;
};

GPC_AV_NAMESPACE_END

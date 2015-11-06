#pragma once

#include <memory>
#include <string>

#ifndef DONT_USE_GPC_AV_DEMUXER
#include "_av/Demuxer.hpp"
#endif
#ifndef DONT_USE_GPC_AV_VIDEO_DECODER
#include "_av/VideoDecoder.hpp"
#endif
#ifndef DONT_USE_GPC_AV__VIDEO_PLAYER
#include "_av/Player.hpp"
#endif
#include "_av/config.hpp"
#ifdef USE_GPC_AV_OPENGL
#include "_av/opengl/YUVPainter.hpp"
#endif

GPC_AV_NAMESPACE_START
    
/* The following is copied from avutil.h
 */
enum AVMediaType
{
    UNKNOWN = -1    ///< Usually treated DATA
  , VIDEO
  , AUDIO
  , DATA            ///< Opaque data information usually continuous
  , SUBTITLE
  , ATTACHMENT      ///< Opaque data information usually sparse
  , NB    
}; 

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

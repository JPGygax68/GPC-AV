#include <gpc/_av/CodecIDs.hpp>

extern "C" {
#include "libavcodec/avcodec.h"
}


GPC_AV_NAMESPACE_START

// TODO: see header file CodecIDs.hpp

const int CodecID::H264 = AV_CODEC_ID_H264;

GPC_AV_NAMESPACE_END
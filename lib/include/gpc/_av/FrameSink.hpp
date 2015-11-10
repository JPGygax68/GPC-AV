#pragma once

#include "config.hpp"

GPC_AV_NAMESPACE_START

class Frame;

/** This is the interface through which decoders deliver frames to consumers.
 */
class FrameSink {
public:
    
    virtual bool ready() = 0;
};

GPC_AV_NAMESPACE_END


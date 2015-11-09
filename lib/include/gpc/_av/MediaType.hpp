#pragma once

#include "config.hpp"

GPC_AV_NAMESPACE_START

/* The following is copied from avutil.h
*/
enum MediaType
{
    UNKNOWN = -1    ///< Usually treated DATA
    , VIDEO
    , AUDIO
    , DATA            ///< Opaque data information usually continuous
    , SUBTITLE
    , ATTACHMENT      ///< Opaque data information usually sparse
    , NB    
}; 

GPC_AV_NAMESPACE_END

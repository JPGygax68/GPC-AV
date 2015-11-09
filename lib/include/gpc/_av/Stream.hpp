#pragma once

#include "Rational.hpp"
#include "MediaType.hpp"

#include "config.hpp"

struct AVStream;

GPC_AV_NAMESPACE_START

/** This class represents a Stream as provided by a Demuxer.

    Technical note: this is a thin wrapper around the AVStream struct and associated API
    functions. It does not have any data members of its own and, most importantly, 
    destroying an instance does NOT destroy, deallocate or unref the underlying AVStream 
    structure.

    Note: in a future extension, this class might also encompass a Demuxer reference or
        pointer, so that user code may subscribe for packet delivery via the stream
        rather than go back to the demuxer. However, Stream will remain a thin wrapper
        class, and thus the actual pub/sub mechanism shall be kept in Demuxer, with
        Stream acting as a delegator.
 */
class Stream {
public:
    auto media_type() const -> MediaType;
    auto time_base() const -> Rational;

protected:

    explicit Stream(AVStream *);

    AVStream   *stream;
};

GPC_AV_NAMESPACE_END

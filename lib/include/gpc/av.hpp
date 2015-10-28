#pragma once

#include <memory>
#include <string>

#ifndef NO_DEMUXER
#include "_av/Demuxer.hpp"
#endif
#ifndef NO_STREAM
#include "_av/Stream.hpp"
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

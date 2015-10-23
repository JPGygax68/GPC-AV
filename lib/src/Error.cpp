extern "C" {
#include <libavutil/error.h>
}

#include <gpc/_av/config.hpp>

#include <gpc/_av/error.hpp>

GPC_AV_NAMESPACE_START

auto Error::make_message(int code, const std::string &context) -> std::string
{
    char buffer[1024];

    av_strerror(code, buffer, sizeof(buffer));

    std::string msg = buffer;
    if (!context.empty()) msg += " " + context;

    return msg;
}

GPC_AV_NAMESPACE_END

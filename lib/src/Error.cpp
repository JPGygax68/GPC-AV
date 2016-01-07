extern "C" {
#include <libavutil/error.h>
}

#include <gpc/_av/config.hpp>

#include <gpc/_av/error.hpp>

GPC_AV_NAMESPACE_START

auto get_error_text(int code) -> std::string
{
    char buffer[1024];

    av_strerror(code, buffer, sizeof(buffer));
    buffer[1023] = '\0'; // just to be safe

    return buffer;
}

auto Error::make_message(int code, const std::string &context) -> std::string
{
    auto msg = get_error_text(code);

    if (!context.empty()) msg += " " + context;

    return msg;
}

GPC_AV_NAMESPACE_END

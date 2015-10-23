#pragma warning(push)
#pragma warning(disable:4244)
extern "C" {
#include <libavformat/avformat.h>
}
#pragma warning(pop)

#include <gpc/_av/config.hpp>

#include "../include/gpc/av" // TODO: this may not be the best way to obtain the contract

GPC_AV_NAMESPACE_START
    
// PIMPL DECLARATION ----------------------------------------

struct Source::Private {
    void _open(const std::string &url);
};

// PUBLIC INTERFACE -----------------------------------------

Source::Source(): p(new Private()) {}

Source::Source(Source&& orig): p(std::move(orig.p)) {}

Source::~Source() {}

auto Source::create(const std::string &url) -> Source
{
    Source source;

    source.open(url);

    return source;
}
        
void Source::open(const std::string &url)
{
}

// PIMPL IMPLEMENTATION -------------------------------------

static struct Initializer {
    Initializer() { av_register_all(); }
} initializer;

void Source::Private::_open(const std::string &url)
{
    
}

GPC_AV_NAMESPACE_END

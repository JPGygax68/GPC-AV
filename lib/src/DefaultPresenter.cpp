#include <gpc/_av/DefaultPresenter.hpp>

GPC_AV_NAMESPACE_START

// PIMPL DECLARATION ------------------------------------------------

// TODO: inherit from a PresenterImplBase class ?

struct DefaultPresenter::Impl {

    Impl(Demuxer& demux, PresentCallback cb);

    Demuxer            &demuxer;
    PresentCallback     callback;
};

// PUBLIC INTERFACE -------------------------------------------------

DefaultPresenter::DefaultPresenter(Demuxer &demux, PresentCallback cb):
    p(new Impl(demux, cb))
{
}

// IMPLEMENTATION (PIMPL) -------------------------------------------

DefaultPresenter::Impl::Impl(Demuxer& demux, PresentCallback cb):
    demuxer(demux),
    callback(cb)
{
}

GPC_AV_NAMESPACE_END


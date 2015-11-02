#include <functional>

#include <gpc/_av/Decoder.hpp>

#include <gpc/_av/DefaultPresenter.hpp>

GPC_AV_NAMESPACE_START

using namespace std;

// PIMPL DECLARATION ------------------------------------------------

class Decoder;

// TODO: inherit from a PresenterImplBase class ?

struct DefaultPresenter::Impl {

    Impl(Decoder &dec, PresentCallback cb);
    ~Impl();

    void play();
    void pause();
    void stop();

    void check_new_frame(const Frame &);

    Decoder            &decoder;
    PresentCallback     callback;

    int                 dec_token;
};

// PUBLIC INTERFACE -------------------------------------------------

DefaultPresenter::DefaultPresenter(Decoder &dec, PresentCallback cb):
    p(new Impl(dec, cb))
{
}

void DefaultPresenter::play()
{
    p->play();
}

void DefaultPresenter::pause()
{
    p->pause();
}

void DefaultPresenter::stop()
{
    p->stop();
}

// IMPLEMENTATION (PIMPL) -------------------------------------------

DefaultPresenter::Impl::Impl(Decoder &dec, PresentCallback cb):
    decoder(dec),
    callback(cb)
{
    using namespace std::placeholders;

    dec_token = decoder.add_consumer(bind(&Impl::check_new_frame, this, _1));
}

DefaultPresenter::Impl::~Impl()
{
    decoder.remove_consumer(dec_token);
}

void DefaultPresenter::Impl::play()
{
}

void DefaultPresenter::Impl::pause()
{
}

void DefaultPresenter::Impl::stop()
{
}

GPC_AV_NAMESPACE_END


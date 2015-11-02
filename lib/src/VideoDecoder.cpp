#include <cassert>
#include <deque>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

#include <gpc/_av/Frame.hpp>

#include <gpc/_av/VideoDecoder.hpp>
#include <gpc/_av/internal/DecoderBase_Impl.hpp>
#include "checked_calls.hpp"

GPC_AV_NAMESPACE_START

using namespace std;

// GLOBAL PRIVATE CONSTANTS -----------------------------------------

static const size_t MAX_BUFFERED_FRAMES = 10;

// PRIVATE CLASSES --------------------------------------------------

// PIMPL DECLARATION ------------------------------------------------

struct VideoDecoder::Impl: public DecoderBase::Impl {

    AVCodecContext         *context;
    AVCodec                *codec;
    bool                    init_done;
    AVFrame                *frame;
    int                     got_frame;

    Impl(AVCodecContext *ctx_, AVCodec *codec_);
    ~Impl();

    void initialize();
    void cleanup();

    bool decode_packet(AVPacket *packet);       // Returns true if a frame was obtained
    //bool frame_available();
    //auto get_nextframe() -> Frame;
    //void recycle_frame(Frame &&frame);

    friend class VideoDecoderImpl;
};

// PUBLIC METHODS ---------------------------------------------------

VideoDecoder::VideoDecoder(Impl *pimpl): DecoderBase(static_cast<DecoderBase::Impl*>(pimpl)) {}

VideoDecoder::~VideoDecoder() = default;

// FRIEND-CALLABLE METHODS ------------------------------------------

auto VideoDecoder::createFromStream(void *stream_) -> VideoDecoder *
{
    auto stream = static_cast<AVStream*>(stream_);
    auto dec_ctx = stream->codec;

    // Find the decoder for the stream
    auto dec = _av(avcodec_find_decoder, dec_ctx->codec_id);

    // Open the decoder
    _av(avcodec_open2, dec_ctx, dec, nullptr);

    // Create an implementation
    // TODO: may need the stream as well
    auto impl = new VideoDecoder::Impl(dec_ctx, dec);

    // Finally, create and return the VideoDecoder instance
    return new VideoDecoder(impl);
}

void VideoDecoder::initialize()
{
    p()->initialize();
}

void VideoDecoder::cleanup()
{
    p()->cleanup();
}

bool VideoDecoder::decode_packet(void * packet)
{
    return p()->decode_packet(static_cast<AVPacket*>(packet));
}

auto VideoDecoder::p() -> Impl*
{
    return static_cast<Impl*>(_p.get()); 
}

// PIMPL IMPLEMENTATION ---------------------------------------------

VideoDecoder::Impl::Impl(AVCodecContext *ctx_, AVCodec *codec_):
    context(ctx_), codec(codec_), init_done(false)
{
}

VideoDecoder::Impl::~Impl()
{ 
    cleanup(); 

    if (context) {
        _av(avcodec_close, context);
        context = nullptr;
    }
}

void VideoDecoder::Impl::initialize()
{
    if (!init_done)
    {
        //for (auto i = 0U; i < MAX_BUFFERED_FRAMES; i++) queued_frames.push_back(_av(av_frame_alloc));
        frame = _av(av_frame_alloc);
        got_frame = 0;

        init_done = true;
    }
}

void VideoDecoder::Impl::cleanup()
{
    if (init_done)
    {
        //for (auto i = 0U; i < MAX_BUFFERED_FRAMES; i++) av_frame_free(&queued_frames[i]);
        //queued_frames.clear();
        av_frame_free(&frame);
        init_done = false;
    }
}

bool VideoDecoder::Impl::decode_packet(AVPacket * packet)
{
    assert(init_done);

    _av(avcodec_decode_video2, context, frame, &got_frame, packet);

    if (got_frame)
    {
        // Move frame from "queued" to "ready" queues
        //lock_guard<mutex> lk(frames_mutex);
        //ready_frames.push_back(queued_frames.front());
        //queued_frames.pop_front();

        deliver_frame(Frame(frame));

        got_frame = 0;
        return true;
    }
    else return false;
}

/*
bool VideoDecoder::Impl::frame_available()
{
    lock_guard<mutex> lk(frames_mutex);

    return ! ready_frames.empty();
}

auto VideoDecoder::Impl::get_nextframe() -> Frame
{
    lock_guard<mutex> lk(frames_mutex);

    auto frame = ready_frames.front();
    ready_frames.pop_front();

    return Frame(frame);
}

void VideoDecoder::Impl::recycle_frame(Frame &&frame)
{
    lock_guard<mutex> lk(frames_mutex);

    queued_frames.push_back(frame.frame);
}
*/

GPC_AV_NAMESPACE_END

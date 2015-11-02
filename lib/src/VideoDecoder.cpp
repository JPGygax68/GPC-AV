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
#include <gpc/_av/internal/DecoderBaseImpl.hpp>
#include "checked_calls.hpp"

GPC_AV_NAMESPACE_START

using namespace std;

// GLOBAL PRIVATE CONSTANTS -----------------------------------------

static const size_t MAX_BUFFERED_FRAMES = 10;

// PRIVATE CLASSES --------------------------------------------------

// PIMPL DECLARATION ------------------------------------------------

struct VideoDecoder_Impl: public DecoderBase::Impl {

    AVCodecContext         *context;
    AVCodec                *codec;
    deque<AVFrame*>         queued_frames;      // Frames available for filling (front is being filled now or next)
    deque<AVFrame*>         ready_frames;       // Frames ready for consumption
    mutex                   frames_mutex;       // Lock this to modify either of the above deques
    int                     got_frame;

    VideoDecoder_Impl(AVCodecContext *ctx_, AVCodec *codec_);
    ~VideoDecoder_Impl();

    void initialize();
    void cleanup();

    bool decode_packet(AVPacket *packet);       // Returns true if a frame was obtained
    //bool frame_available();
    //auto get_nextframe() -> Frame;
    //void recycle_frame(Frame &&frame);

    friend class VideoDecoderImpl;
};

// PUBLIC METHODS ---------------------------------------------------

VideoDecoder::VideoDecoder(Impl *pimpl): Decoder<VideoDecoder, VideoDecoder_Impl>(pimpl) {}

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
    auto impl = new VideoDecoder_Impl(dec_ctx, dec);

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

/*
bool VideoDecoder::frame_available()
{
    return p->frame_available();
}

auto VideoDecoder::get_next_frame() -> Frame
{
    return p->get_nextframe();
}

void VideoDecoder::recycle_frame(Frame &&frame)
{
    p->recycle_frame(std::move(frame));
}
*/

// PIMPL IMPLEMENTATION ---------------------------------------------

VideoDecoder_Impl::VideoDecoder_Impl(AVCodecContext *ctx_, AVCodec *codec_):
    context(ctx_), codec(codec_) 
{
}

VideoDecoder_Impl::~VideoDecoder_Impl()
{ 
    cleanup(); 
}

void VideoDecoder_Impl::initialize()
{
    for (auto i = 0U; i < MAX_BUFFERED_FRAMES; i++) queued_frames.push_back(_av(av_frame_alloc));

    got_frame = 0;
}

void VideoDecoder_Impl::cleanup()
{
    if (context) {
        _av(avcodec_close, context);
        context = nullptr;
    }
}

bool VideoDecoder_Impl::decode_packet(AVPacket * packet)
{
    _av(avcodec_decode_video2, context, queued_frames.front(), &got_frame, packet);

    if (got_frame)
    {
        // Move frame from "queued" to "ready" queues
        lock_guard<mutex> lk(frames_mutex);
        ready_frames.push_back(queued_frames.front());
        queued_frames.pop_front();
        // TODO: send signal to "subscribed" consumers?
        got_frame = 0;
        return true;
    }
    else return false;
}

/*
bool VideoDecoder_Impl::frame_available()
{
    lock_guard<mutex> lk(frames_mutex);

    return ! ready_frames.empty();
}

auto VideoDecoder_Impl::get_nextframe() -> Frame
{
    lock_guard<mutex> lk(frames_mutex);

    auto frame = ready_frames.front();
    ready_frames.pop_front();

    return Frame(frame);
}

void VideoDecoder_Impl::recycle_frame(Frame &&frame)
{
    lock_guard<mutex> lk(frames_mutex);

    queued_frames.push_back(frame.frame);
}
*/

GPC_AV_NAMESPACE_END

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

#include <gpc/_av/VideoFrame.hpp>

#include <gpc/_av/VideoDecoder.hpp>
#include <gpc/_av/internal/Decoder.ipp>
#include <gpc/_av/internal/Decoder_Impl.hpp>
#include "checked_calls.hpp"

GPC_AV_NAMESPACE_START

using namespace std;

// GLOBAL PRIVATE CONSTANTS -----------------------------------------

static const size_t MAX_BUFFERED_FRAMES = 10;

// PIMPL DECLARATION ------------------------------------------------

struct VideoDecoder_Impl: public Decoder_Impl<VideoDecoder> {

    bool                    init_done;
    AVFrame                *frame;
    int                     got_frame;

    VideoDecoder_Impl(AVCodecContext *ctx_, AVCodec *codec_);
    ~VideoDecoder_Impl();

    void initialize();
    void cleanup();

    bool decode_packet(AVPacket *packet);       // Returns true if a frame was obtained

    friend class VideoDecoderImpl;
};

// Now we can (explicitly) instantiate base class template
template class Decoder<VideoDecoder, VideoFrame, VideoDecoder_Impl>;

// PUBLIC METHODS ---------------------------------------------------

VideoDecoder::VideoDecoder(VideoDecoder_Impl *pimpl): 
    Decoder(pimpl)
{
}

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

// PIMPL IMPLEMENTATION ---------------------------------------------

VideoDecoder_Impl::VideoDecoder_Impl(AVCodecContext *ctx_, AVCodec *codec_):
    Decoder_Impl(ctx_, codec_), init_done(false)
{
}

VideoDecoder_Impl::~VideoDecoder_Impl()
{ 
    cleanup(); 

    if (context) {
        _av(avcodec_close, context);
        context = nullptr;
    }
}

void VideoDecoder_Impl::initialize()
{
    if (!init_done)
    {
        //for (auto i = 0U; i < MAX_BUFFERED_FRAMES; i++) queued_frames.push_back(_av(av_frame_alloc));
        frame = _av(av_frame_alloc);
        got_frame = 0;

        init_done = true;
    }
}

void VideoDecoder_Impl::cleanup()
{
    if (init_done)
    {
        //for (auto i = 0U; i < MAX_BUFFERED_FRAMES; i++) av_frame_free(&queued_frames[i]);
        //queued_frames.clear();
        av_frame_free(&frame);
        init_done = false;
    }
}

bool VideoDecoder_Impl::decode_packet(AVPacket * packet)
{
    assert(init_done);

    _av(avcodec_decode_video2, context, frame, &got_frame, packet);

    if (got_frame)
    {
        // Move frame from "queued" to "ready" queues
        //lock_guard<mutex> lk(frames_mutex);
        //ready_frames.push_back(queued_frames.front());
        //queued_frames.pop_front();

        deliver_frame(VideoFrame(frame));

        got_frame = 0;
        return true;
    }
    else return false;
}

GPC_AV_NAMESPACE_END

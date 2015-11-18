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

#include <gpc/_av/VideoStream.hpp>
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

struct VideoDecoder::Impl: public Decoder<VideoDecoder, VideoFrame>::Impl {

    int                     got_frame;

    Impl(AVCodecContext *ctx_, AVCodec *codec_);
    ~Impl();

    auto decode_packet(AVPacket *packet) -> bool; // true if a frame was completed

    friend class VideoDecoderImpl;
};

// Now we can (explicitly) instantiate base class template
template class Decoder<VideoDecoder, VideoFrame>;

// PUBLIC METHODS ---------------------------------------------------

VideoDecoder::VideoDecoder(Impl *pimpl): 
    Decoder(pimpl)
{
}

VideoDecoder::~VideoDecoder() = default;

// FRIEND-CALLABLE METHODS ------------------------------------------

auto VideoDecoder::create_from_stream(const VideoStream &stream) -> VideoDecoder *
{
    auto dec_ctx = stream.stream->codec;

    // Find the decoder for the stream
    auto dec = _av(avcodec_find_decoder, dec_ctx->codec_id);

    // Open the decoder
    _av(avcodec_open2, dec_ctx, dec, nullptr);

    // Create an implementation
    // TODO: may need the stream as well
    auto impl = new Impl(dec_ctx, dec);

    // Finally, create and return the VideoDecoder instance
    return new VideoDecoder(impl);
}

bool VideoDecoder::decode_packet(void * packet)
{
    return p()->decode_packet(static_cast<AVPacket*>(packet));
}

auto VideoDecoder::p() -> Impl *
{
    return static_cast<Impl*>(_p.get());
}

// PIMPL IMPLEMENTATION ---------------------------------------------

VideoDecoder::Impl::Impl(AVCodecContext *ctx_, AVCodec *codec_):
    Decoder<VideoDecoder, VideoFrame>::Impl(ctx_, codec_), got_frame(0)
{
}

VideoDecoder::Impl::~Impl()
{ 
    av_frame_free(&frame);

    if (context) {
        _av(avcodec_close, context);
        context = nullptr;
    }
}

bool VideoDecoder::Impl::decode_packet(AVPacket * packet)
{
    //_av(avcodec_decode_video2, context, frame, &got_frame, packet);
    int err = avcodec_decode_video2(context, frame, &got_frame, packet);
        // TODO: handle error

    if (got_frame)
    {
        if (frame->pts == 0 && frame->pkt_pts == 0)
        {
            // Dirty hack - then again, FFmpeg is supposed to do that on its own
            frame->pts = packet->pts;
        }
    }

    return got_frame != 0;
}

GPC_AV_NAMESPACE_END

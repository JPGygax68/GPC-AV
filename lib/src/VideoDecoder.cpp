#include <cassert>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
extern "C" {
#include "libavformat/avformat.h"
}

#include <gpc/_av/VideoDecoder.hpp>
#include "checked_calls.hpp"

GPC_AV_NAMESPACE_START

// GLOBAL PRIVATE CONSTANTS -----------------------------------------

static const size_t FRAME_QUEUE_SIZE = 10;

// PRIVATE CLASSES --------------------------------------------------

class FrameQueue {
public:

    void initialize()
    {
        assert(queue.empty());

        for (auto i = 0U; i < FRAME_QUEUE_SIZE; i++) queue.push_back(_av(av_frame_alloc));
        for (auto i = 0U; i < FRAME_QUEUE_SIZE; i++) writeable_cv.notify_one();
    }

    void cleanup()
    {
        for (auto f : queue) av_free(f);
        queue.resize(0);
    }

    /*  Returns the next frame that is available for writing (i.e. for *produc*ing).
        This function will wait on the condition variable.
        Every call to get_next_writeable_frame() must be followed by a call to
        frame_filled(), except when the queue is being torn down.
        IMPORTANT: every call to get_next_writeable_frame() must be followed
        by a call to frame_filled(), except when the queue is being torn down.
     */
    auto get_next_writeable_frame() -> AVFrame*
    {
        // Wait for a Frame to become available for writing
        std::unique_lock<std::mutex> lk(mutex);
        writeable_cv.wait(lk, [this]() { return tearing_down || writeable_count > 0; });
        if (tearing_down) return nullptr;

        // Update the count and return the frame
        writeable_count--;
        return queue[writeable_index];
    }

    /** The producer MUST call this after it has filled the frame it obtained
        via get_next_writeable_frame().
     */
    void frame_filled()
    {
        // Move on to next writeable frame
        writeable_index++;
        writeable_index %= queue.size();

        // Frame available for consumption
        readable_count++;
        readable_cv.notify_one();
    }

    /** Returns the next frame ready for reading (consumption).
        Will suspend the caller until a frame becomes available or the queue
        is being torn down, in which case it will return nullptr.
        IMPORTANT: each call to get_next_readable_frame() must be followed
        by a call to frame_consumed(), except when the queue is being torn
        down.
        */
    auto get_next_readable_frame() -> AVFrame*
    {
        // Wait for a Frame to become available for writing
        std::unique_lock<std::mutex> lk(mutex);
        readable_cv.wait(lk, [this]() { return tearing_down || readable_count > 0; });
        if (tearing_down) return nullptr;

        // Update the count and return the frame
        readable_count--;
        return queue[readable_index];
    }

    /** The consumer MUST call this after it is done with the frame obtained
        via get_next_readable_frame().
     */
    void frame_consumed()
    {
        // Move on to next writeable frame
        readable_index++;
        readable_index %= queue.size();

        // Frame available for consumption
        writeable_count++;
        writeable_cv.notify_one();
    }

private:
    std::vector<AVFrame*>       queue;
    unsigned int                writeable_index;
    unsigned int                readable_index;
    std::condition_variable     writeable_cv;
    std::condition_variable     readable_cv;
    unsigned int                readable_count;
    unsigned int                writeable_count;
    std::mutex                  mutex;
    bool                        tearing_down;
};

// PIMPL DECLARATION ------------------------------------------------

struct VideoDecoder::Impl {

    AVCodecContext         *context;
    AVCodec                *codec;
    FrameQueue              frame_queue;

    Impl(AVCodecContext *ctx_, AVCodec *codec_);
    ~Impl();

    void close();

    friend class VideoDecoderImpl;
};

// PUBLIC METHODS ---------------------------------------------------

VideoDecoder::VideoDecoder(Impl *pimpl): p(pimpl) {}

VideoDecoder::~VideoDecoder() = default;

VideoDecoder::VideoDecoder(VideoDecoder&& from)
{
    p.swap(from.p);
}

VideoDecoder & VideoDecoder::operator = (VideoDecoder&& from)
{
    p.swap(from.p);

    return *this;
}

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

// PIMPL IMPLEMENTATION ---------------------------------------------

VideoDecoder::Impl::Impl(AVCodecContext *ctx_, AVCodec *codec_): 
    context(ctx_), codec(codec_) 
{
}

VideoDecoder::Impl::~Impl()
{ 
    close(); 
}

void VideoDecoder::Impl::close()
{
    if (context) {
        _av(avcodec_close, context);
        context = nullptr;
    }
}

GPC_AV_NAMESPACE_END

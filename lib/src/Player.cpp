#include "..\include\gpc\_av\Player.hpp"
#include <functional>
#include <deque>
#include <thread>
#include <mutex>
#include <chrono>
#include <cassert>
#include <iostream>

#include <gpc/_av/Demuxer.hpp>
#include <gpc/_av/VideoDecoder.hpp>
#include <gpc/_av/VideoStream.hpp>
#include <gpc/_av/VideoFrame.hpp>
#include <gpc/_av/internal/HighResClock.hpp>
#include <gpc/Rational.hpp>

#include <gpc/_av/Player.hpp>

GPC_AV_NAMESPACE_START

using namespace std;

// IMPLEMENTATION CLASS DECLARATION (PIMPL) -------------------------

struct Player::Impl {

    static const size_t MIN_VIDEO_INITIAL_QUEUE_SIZE = 5;
    static const size_t TARGET_VIDEO_QUEUE_SIZE = 8;
    static const size_t MAX_VIDEO_QUEUE_SIZE = 10;              // TODO: replace with setting and/or dynamic value

    class VideoSink : public VideoDecoder::ISink {
    public:
        VideoSink(Player::Impl &owner);
        ~VideoSink();

        auto ready() -> bool override;
        void process_frame(const VideoFrame &) override;

    private:
        friend class Player;
        friend struct Player::Impl;

        Player::Impl       &owner;
        deque<VideoFrame>   frame_queue;
        mutex               queue_mutex;
    };

    enum State { UNDEFINED = 0, BUFFERING, PLAYING, PAUSED, FINISHED };

    //typedef chrono::high_resolution_clock   clock_t;
    typedef ::gpc::HighResClock             clock_t;
    typedef clock_t::duration               duration_t;
    typedef clock_t::time_point             timepoint_t;

    Impl();
    
    void open(const std::string &url);

    void play();
    void pause();
    void stop();

    auto process_video_frame(const VideoFrame &) -> bool;

    auto peek_newest_video_frame() -> const VideoFrame *;
    auto current_video_frame() -> const VideoFrame *;

    void suspend_demuxing();
    void try_resume_demuxing();

    void added_new_video_frame();
    void added_new_audio_frame();

    Demuxer             demuxer;
    VideoSink           video_sink;
    // TODO: audio sink
    clock_t             clock;
    State               state;
    timepoint_t         starting_timepoint;
    size_t              stall_count;

    Rational            last_pts;
    Rational            greatest_pts_lapse;
};

// PUBLIC METHODS ---------------------------------------------------

auto Player::create(const std::string& url) -> Player*
{
    Player *player = new Player();

    player->p->open(url);

    return player;
}

Player::Player() : p(new Impl()) {}

Player::~Player() = default;

void Player::open(const std::string & url) { p->open(url); }

void Player::play() { p->play(); }

void Player::pause() { p->pause(); }

void Player::stop()
{
    p->stop();
}

auto Player::peek_newest_video_frame() -> const VideoFrame *
{
    return p->peek_newest_video_frame();
}

bool Player::video_frame_available()
{
    return !p->video_sink.frame_queue.empty();
}

auto Player::current_video_frame() -> const VideoFrame *
{
    return p->current_video_frame();
}

// PRIVATE / PROTECTED METHODS --------------------------------------

// IMPLEMENTATION (PIMPL) -------------------------------------------

Player::Impl::Impl():
    state(UNDEFINED),
    video_sink{*this}
{
}

void Player::Impl::open(const std::string & url)
{
    using namespace placeholders;

    stall_count = 0;

    demuxer.open(url);

    // TODO: move this into a yet-to-be-defined Demuxer callback because streams may not be 
    // known right after opening, if data comes via a network
    // TODO: check if a video stream exists (video_decoder() will throw otherwise)
    demuxer.video_decoder().add_sink(video_sink); // consumer(bind(&Impl::process_video_frame, this, _1));
}

void Player::Impl::play()
{
    demuxer.start();
    state = BUFFERING; // TODO: notify subscribers ?

    last_pts = { 0, 1 };
    greatest_pts_lapse = { 0, 1 };
}

void Player::Impl::pause()
{
    if (state == PLAYING)
    {
        demuxer.suspend();
        state = PAUSED; // TODO: notify subscribers ?
    }
}

void Player::Impl::stop()
{
    demuxer.stop();
}

auto Player::Impl::peek_newest_video_frame() -> const VideoFrame *
{
    return !video_sink.frame_queue.empty() ? &video_sink.frame_queue.front() : nullptr;
}

auto Player::Impl::current_video_frame() -> const VideoFrame *
{
    class ProfilingTimer {
    public:
        ProfilingTimer(clock_t &clock_): clock(clock_)
        {
            start = clock.now();
        }
        ~ProfilingTimer()
        {
            auto now = clock.now();
            if (Rational {5, 1000} < (now - start))
            {
                cerr << "Player::current_video_frame(): execution took too long:" << (now - start).count() << endl;
            }
        }
        private:
            clock_t &clock;
            clock_t::time_point start;
    };

    /*
    static clock_t::time_point last_time;
    static bool already_called = false;

    if (!already_called) 
    {
        already_called = true;
    }
    else
    {
        auto now = clock.now();
        auto elapsed = now - last_time;
        if (Rational{ 3, 24*2 } < elapsed) cerr << "More than one frame duration has elapsed since last call" << endl;
        last_time = now;
    }
    */

    unique_lock<mutex> lk(video_sink.queue_mutex);

    if (video_sink.frame_queue.empty())
    {
        return nullptr;
    }
    else 
    {
        auto now = clock.now();

        auto &video_stream = demuxer.video_stream();
        // TODO: audio stream, others

        if (state == BUFFERING && (video_sink.frame_queue.size() >= MIN_VIDEO_INITIAL_QUEUE_SIZE || demuxer.stream_ended()))
        {
            starting_timepoint = now;
            state = PLAYING; // TODO: notification
        }

        if (state == PLAYING)
        {
            auto curr_time = now - starting_timepoint;

            // Eliminate elapsed frames (but keep at least one, even if elapsed)
            // TODO: handle end of stream from demuxer
            while (video_sink.frame_queue.size() > 1)
            {
                auto &frame = video_sink.frame_queue.front();

                // If not elapsed, stop
                auto presentation_time = frame.presentation_timestamp() * video_stream.time_base();
                if (presentation_time > curr_time)
                    break;

                // Remove from queue
                video_sink.frame_queue.pop_front();

                // TODO: replace with adaptive number (e.g. 1/4 of maximum queue size?)
                if (video_sink.frame_queue.size() < 4) cerr << "Player:: frame queue size dropped below 4" << endl;

                // Tell demuxer to resume
                if (demuxer.is_suspended())
                {
                    demuxer.resume();
                }
            }

            // Because frames can be out of order, find the "earliest" among the non-expired
            auto best_frame = &video_sink.frame_queue.front();
            for (auto it = begin(video_sink.frame_queue) + 1; it != end(video_sink.frame_queue); it++)
            {
                if (it->presentation_timestamp() < best_frame->presentation_timestamp())
                {
                    best_frame = &(*it);
                }
            }

            /*
            auto curr_pts = best_frame->presentation_timestamp() * video_stream.time_base();
            auto lapse = curr_pts - last_pts;
            if (Rational{ 11, 240 } < lapse)
            {
                cerr << "Lapse between frames greater than timebase: " << lapse << endl;
            }
            last_pts = curr_pts;
            */

            return best_frame;
        }
        else
        {
            return nullptr;
        }
    }
}

void Player::Impl::suspend_demuxing()
{
    if (stall_count++ > 0)
    {
        demuxer.suspend();
    }
}

void Player::Impl::try_resume_demuxing()
{
    assert(stall_count > 0);

    if (--stall_count == 0)
    {
        demuxer.resume();
    }
}

void Player::Impl::added_new_video_frame()
{
    // TODO
}

void Player::Impl::added_new_audio_frame()
{
    // TODO
}

// IMPL SUBCLASS: VideoSink -----------------------------------------

Player::Impl::VideoSink::VideoSink(Player::Impl &owner_):
    owner(owner_)
{
}

Player::Impl::VideoSink::~VideoSink()
{
    lock_guard<mutex> lk(queue_mutex);
}

auto Player::Impl::VideoSink::ready() -> bool
{
    unique_lock<mutex> lk(queue_mutex);

    return frame_queue.size() < MAX_VIDEO_QUEUE_SIZE;
}

void Player::Impl::VideoSink::process_frame(const VideoFrame &frame)
{
    lock_guard<mutex> lk(queue_mutex);

    frame_queue.emplace_back(frame);

    owner.added_new_video_frame();
}

GPC_AV_NAMESPACE_END


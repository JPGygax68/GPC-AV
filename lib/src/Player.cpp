#include <functional>
#include <deque>
#include <thread>
#include <mutex>
#include <chrono>
#include <cassert>

#include <gpc/_av/Demuxer.hpp>
#include <gpc/_av/VideoDecoder.hpp>
#include <gpc/_av/VideoFrame.hpp>

#include <gpc/_av/Player.hpp>

GPC_AV_NAMESPACE_START

using namespace std;

// IMPLEMENTATION CLASS DECLARATION (PIMPL) -------------------------

struct Player::Impl {

    typedef chrono::high_resolution_clock   clock_t;
    typedef clock_t::duration               duration_t;
    typedef clock_t::time_point             timepoint_t;

    static const size_t MAX_VIDEO_QUEUE_SIZE = 2;   // TODO: replace with setting and/or dynamic value

    Impl();
    
    void open(const std::string &url);

    void play();
    void pause();

    void process_video_frame(const VideoFrame &);

    auto peek_newest_video_frame() -> const VideoFrame *;
    auto get_newest_video_frame() -> VideoFrame;
    auto current_video_frame() -> const VideoFrame *;

    void suspend_demuxing();
    void try_resume_demuxing();

    Demuxer             demuxer;
    deque<VideoFrame>   video_queue;
    mutex               queues_mutex;
    clock_t             clock;
    timepoint_t         starting_timepoint;
    size_t              stall_count;
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

auto Player::peek_newest_video_frame() -> const VideoFrame *
{
    return p->peek_newest_video_frame();
}

auto Player::get_newest_video_frame() -> VideoFrame
{
    return p->get_newest_video_frame();
}

bool Player::video_frame_available()
{
    return !p->video_queue.empty();
}

auto Player::current_video_frame() -> const VideoFrame *
{
    return p->current_video_frame();
}

// PRIVATE / PROTECTED METHODS --------------------------------------

// IMPLEMENTATION (PIMPL) -------------------------------------------

Player::Impl::Impl()
{
}

void Player::Impl::open(const std::string & url)
{
    using namespace placeholders;

    stall_count = 0;

    demuxer.open(url);

    // TODO: move this into a yet-to-be-defined Demuxer callback because streams may not be 
    // known right after opening, if data comes via a network

    demuxer.video_decoder().add_consumer(bind(&Impl::process_video_frame, this, _1));
}

void Player::Impl::play()
{
    // TODO
}

void Player::Impl::pause()
{
    // TODO
}

// TODO: replace this method with a generic one that works with any type of frame ?

void Player::Impl::process_video_frame(const VideoFrame &frame)
{
    lock_guard<mutex> lk(queues_mutex);

    video_queue.push_back(frame); // TODO: profile this and make sure it's ultra-lightweight

    if (video_queue.size() >= MAX_VIDEO_QUEUE_SIZE) suspend_demuxing();
}

auto Player::Impl::peek_newest_video_frame() -> const VideoFrame *
{
    return !video_queue.empty() ? &video_queue.front() : nullptr;
}

auto Player::Impl::get_newest_video_frame() -> VideoFrame
{
    lock_guard<mutex> lk(queues_mutex);

    bool was_full = video_queue.size() == MAX_VIDEO_QUEUE_SIZE;

    auto &frame = video_queue.front();
    video_queue.pop_front();

    if (was_full) try_resume_demuxing();

    return frame;
}

auto Player::Impl::current_video_frame() -> const VideoFrame *
{
    if (video_queue.empty())
    {
        return nullptr;
    }
    else 
    {
        if (starting_timepoint == timepoint_t())
        {
            starting_timepoint = clock.now();
        }

        auto presentation_time = video_queue[1].presentation_timestamp() * demuxer.video_decoder().time_base();

        auto curr_time = clock.now() - starting_timepoint;

        while (video_queue.size() > 1 && presentation_time < curr_time)
        {
            video_queue.pop_front();
        }

        return &video_queue.front();
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

GPC_AV_NAMESPACE_END

#include <gpc/_av/Demuxer.hpp>

#include <gpc/_av/Player.hpp>

GPC_AV_NAMESPACE_START

// IMPLEMENTATION CLASS DECLARATION (PIMPL) -------------------------

struct Player::Impl {

    Impl();
    
    void open(const std::string &url);

    Demuxer         demuxer;
};

// PUBLIC METHODS ---------------------------------------------------

auto Player::create(const std::string& url) -> Player*
{
    Player *player = new Player();

    player->p->open(url);

    return player;
}

Player::~Player() = default;

void Player::open(const std::string & url)
{
    p->open(url);
}

// PRIVATE / PROTECTED METHODS --------------------------------------

// IMPLEMENTATION (PIMPL) -------------------------------------------

Player::Impl::Impl()
{
}

void Player::Impl::open(const std::string & url)
{
    demuxer.open(url);
}

GPC_AV_NAMESPACE_END

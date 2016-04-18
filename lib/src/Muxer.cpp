#ifdef _WIN32
#include <WinSock2.h> // htons(), htonl() TODO: other platforms
#endif
#include <cassert>
#include <string>
using std::string;
#include <algorithm> // copy
#include <iostream> // debug/trace
#include <vector>

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/intreadwrite.h"
}

#include <gpc/_av/checked_calls.hpp>
#include <gpc/_av/VideoStream.hpp>

#include <gpc/_av/Muxer.hpp>

GPC_AV_NAMESPACE_START

// IMPLEMENTATION STRUCT DECLARATION (PIMPL) ------------------------

struct Muxer::Impl {

    void set_format(const std::string short_name);
    void open(const std::string &url);
    void close();
    auto get_sdp_data() -> std::string;
    void add_video_stream(CodecID, int width, int height);
    auto video_stream() -> VideoStream;
    void write_header();
    void send_packet(int stream_index, const uint8_t * data, int size,
        int64_t pts, int64_t dts, int duration, int pos);
    void send_h264_hevc(int stream_index, const uint8_t * data, int size, 
        int64_t pts, int64_t dts, int duration);

    AVOutputFormat     *output_format = nullptr;
    AVFormatContext    *format_ctx = nullptr;
    AVStream           *video_st = nullptr;
    uint32_t            seq = 0; // for RTP
};

// PUBLIC INTERFACE IMPLEMENTATION ----------------------------------

Muxer::Muxer():
    _p(new Impl())
{
    av_register_all();
    AV(avformat_network_init); // TODO: only do this if necessary
}

Muxer::~Muxer()
{}

void Muxer::set_format(const std::string & short_name)
{
    p()->set_format(short_name);
}

void Muxer::open(const std::string & url)
{
    p()->open(url);
}

void Muxer::add_video_stream(CodecID codec_id, int width, int height)
{
    p()->add_video_stream(codec_id, width, height);
}

auto Muxer::get_sdp_data() -> std::string
{
    return p()->get_sdp_data();
}

auto Muxer::video_stream() -> VideoStream
{
    return p()->video_stream();
}

void Muxer::write_header()
{
    p()->write_header();
}

void Muxer::send_packet(int stream_index, const uint8_t * data, int size, 
    int64_t pts, int64_t dts, int duration, int pos)
{
    p()->send_packet(stream_index, data, size, pts, dts, duration, pos);
}

void Muxer::send_h264_hevc(int stream_index, const uint8_t * data, int size, int64_t pts, int64_t dts, int duration)
{
    p()->send_h264_hevc(stream_index, data, size, pts, dts, duration);
}

void Muxer::close()
{
    p()->close();
}

// HELPER FUNCTIONS -------------------------------------------------

/*
* Find next NAL unit from the specified H.264 bitstream data.
*/
// TODO: remove, this is not needed - FFmpeg does all this for us
static auto find_next_nal_unit(const uint8_t *start, const uint8_t *end) -> const uint8_t *
{
    const uint8_t *p = start;

    /* Simply lookup "0x000001" pattern */
    while (p <= end - 3 && (p[0] || p[1] || p[2] != 1))
    ++p;

    if (p > end - 3)
    /* No more NAL unit in this bitstream */
    return nullptr;

    /* Include 8 bits leading zero */
    if (p > start && *(p - 1) == 0)
    return (p - 1);

    return p;
}

// INTERNAL IMPLEMENTATION (PIMPL) ----------------------------------

void Muxer::Impl::set_format(const std::string short_name)
{
    output_format = AV(av_guess_format, short_name.c_str(), nullptr, nullptr);
}

void Muxer::Impl::open(const std::string &url)
{
    using std::copy;
    using std::begin;

    // Should work for both URLs and filesystem paths:
    string protocol(begin(url), begin(url) + url.find(':'));

    // Guess output format (if not already set)
    if (!output_format)
    {
        if (!protocol.empty())
        {
            // TODO: parse to extract filename
            output_format = av_guess_format(protocol.c_str(), nullptr, nullptr);
        }
        else {
            output_format = nullptr;
        }
    }

    AV(avformat_alloc_output_context2, &format_ctx, output_format, nullptr, url.c_str());

    // Open the output file, if necessary
    if (!(output_format->flags & AVFMT_NOFILE))
    {
        AV(avio_open, &format_ctx->pb, url.c_str(), AVIO_FLAG_WRITE);
        // TODO: question: is it still possible to add streams after that ?
    }

    seq = 0;
}

auto Muxer::Impl::get_sdp_data() -> std::string
{
    char buffer[4096];

    int err = av_sdp_create(&format_ctx, 1, buffer, 4096);
    auto size = strnlen(buffer, 4096);
    return std::string(buffer, size);
}

void Muxer::Impl::add_video_stream(CodecID codec_id, int width, int height)
{
    auto codec = AV(avcodec_find_encoder, (AVCodecID) (int) codec_id);

    video_st = AV(avformat_new_stream, format_ctx, codec);

    auto c = video_st->codec; // codec context

    c->width  = width;
    c->height = height;
    /*
    c->pix_fmt = AV_PIX_FMT_YUV420P; // TODO: make overridable

    _av(avcodec_open2, video_st->codec, codec, nullptr);
    */
}

auto Muxer::Impl::video_stream() -> VideoStream
{
    assert(video_st);

    return VideoStream(video_st);
}

void Muxer::Impl::write_header()
{
    char sdp[2048];

    av_sdp_create(&format_ctx, 1, sdp, sizeof(sdp));
    std::cerr << "SDP:" << std::endl << sdp;

    AV(avformat_write_header, format_ctx, nullptr);
}

void Muxer::Impl::send_packet(int stream_index, const uint8_t * data, int size,
    int64_t pts, int64_t dts, int duration, int pos)
{
    AVPacket pkt;

    av_init_packet(&pkt);

    pkt.data = const_cast<uint8_t*>(data);
    pkt.size = size;
    pkt.pts = pts;
    pkt.dts = dts;
    pkt.duration = duration;
    pkt.pos = pos;

    //_av(av_interleaved_write_frame, format_ctx, &pkt);
    AV(av_write_frame, format_ctx, &pkt);
}

void Muxer::Impl::send_h264_hevc(int stream_index, const uint8_t * data, int size, int64_t pts, int64_t dts, int duration)
{
#ifdef IF_WE_HAD_TO_PACKETIZE_OURSELVES_BUT_WE_DONT
    static const uint8_t RTP_VERSION = 2;

    for (auto pnalu = data; pnalu < data + size; ) 
    {
        assert(pnalu[0] == 0 && pnalu[1] == 0);

        // Find end of this NALU
        auto pend = find_next_nal_unit(pnalu + 3, data + size);
        bool last = pend == nullptr;
        if (!pend) pend = data + size;
        //int nalsz = pend - pnalu;

        // Get NAL unit type
        auto ppl = pnalu;
        while (*ppl++ == 0);
        uint8_t nal_unit_type = *ppl & 0x1f;
        assert(nal_unit_type == 1 || (nal_unit_type >= 5 && nal_unit_type <= 23)); // single NAL packets only!
        //pnalu = ppl;
        int nalsz = pend - pnalu;

        // Packetize NALU
        uint32_t pklen = 12 + 1 + nalsz;
        std::vector<uint8_t> buf(8 + pklen);

        // RTP packet header: length an "in time"
        int i = 0;
        *((uint32_t*)&buf[i]) = htonl(pklen);   // packet length
        i += 4;
        *((int32_t*)&buf[i]) = htonl(-1);  // "in time"
        i += 4;

        // RTP header (RFC 6184): 12 bytes
        buf[i] = (uint8_t) ( 
              ((RTP_VERSION & 0x03) << 6) // v
            | ((0           & 0x01) << 5) // p
            | ((0           & 0x01) << 4) // x
            | ((0           & 0x0f) << 0) // cc
            );
        i += 1;
        buf[i] = (uint8_t) ( 
              ((last        & 0x01) << 7) // m
            | ((96          & 0x7f) << 0) // pt TODO: get from SDP!!!
            );
        i ++;
        *((uint16_t*)&buf[i]) = htons(seq++);
        i += 2;
        *((uint32_t*)&buf[i]) = htonl(pts); // TODO: randomize starting PTS
        i += 4;
        *((uint32_t*)&buf[i]) = htonl(0x12345678); // TODO: better value
        i += 4;

        // NAL unit header
        uint8_t nri;
        switch (nal_unit_type) {
        case 6: case 9: case 10: case 11: case 12:
            nri = 0;
        case 5: // key frame
        case 7: case 8:
            nri = 3;
            break;
        default: // TODO: does not yet cover all possibilities
            nri = 2;
        }
        buf[i] = (uint8_t) (
              ((0             & 0x01) << 7)   // "forbidden" bit
            | ((nri           & 0x03) << 5)   // NRI
            | ((nal_unit_type & 0x3f) << 0)   // NAL unit type
            );
        i += 1;

        for (int j = 0; j < nalsz; j ++) buf[i+j] = pnalu[j];
        //for (int i = 0; i < nalsz; i ++) buf[15+i] = ppl[i];

        // Send the NALU
        //send_packet(video_stream().index(), &buf[0], buf.size(), nal_unit_type == 5, pts, dts, duration, -1);
        send_packet(video_stream().index(), pnalu, pend - pnalu, nal_unit_type == 5, pts, dts, duration, -1);

        pnalu = pend; // on to the next NAL unit
    }
#else
    send_packet(video_stream().index(), data, size, pts, dts, duration, -1);
#endif
}

GPC_AV_NAMESPACE_END

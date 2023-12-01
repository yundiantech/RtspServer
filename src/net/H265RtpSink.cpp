#include <stdio.h>
#include <string.h>

#include "net/H265RtpSink.h"
#include "base/Logging.h"
#include "base/New.h"

#include "h265.h"

#define RTP_HEVC_HEADERS_SIZE 3

H265RtpSink* H265RtpSink::createNew(MediaSource* mediaSource)
{
    if(!mediaSource)
        return NULL;

    return new H265RtpSink(mediaSource);
    // return New<H265RtpSink>::allocate(mediaSource);
}

H265RtpSink::H265RtpSink(MediaSource* mediaSource) :
    RtpSink(mediaSource, RTP_PAYLOAD_TYPE_H264),
    mClockRate(90000),
    mFps(mediaSource->getFps())
{
    start(1000/mFps);
}

H265RtpSink::~H265RtpSink()
{

}

std::string H265RtpSink::getMediaDescription(uint16_t port)
{
    char buf[100] = {0};
    sprintf(buf, "m=video %hu RTP/AVP %d", port, mPayloadType);

    return std::string(buf);
}

std::string H265RtpSink::getAttribute()
{
    char buf[100];
    sprintf(buf, "a=rtpmap:%d H265/%d\r\n", mPayloadType, mClockRate);
    sprintf(buf+strlen(buf), "a=framerate:%d", mFps);

    return std::string(buf);
}

void H265RtpSink::handleFrame(AVFrame* frame)
{
    RtpHeader* rtpHeader = mRtpPacket.mRtpHeadr;

    T_H265_NALU nalu = frame->getVideoFrame()->getNalu()->nalu.h265Nalu;

    const uint8_t *buf = nalu.buf + nalu.startCodeLen;
    int len = nalu.len - nalu.startCodeLen;

//    static FILE *fp1 = fopen("out.h265", "wb");
//    fwrite(nalu.buf, 1, nalu.len, fp1);

    int rtp_payload_size   = RTP_MAX_PKT_SIZE - RTP_HEVC_HEADERS_SIZE;
    int nal_type           = (buf[0] >> 1) & 0x3F;
// printf("%s line=%d nal_type=%d len=%d mTimestamp=%d\n", __FUNCTION__, __LINE__, nal_type, len, mTimestamp);
    //长度<MTU直接打包RTP发送
    /* send it as one single NAL unit? */
    if(len <= RTP_MAX_PKT_SIZE)
    {
        memcpy(rtpHeader->payload, buf, len);
        mRtpPacket.mSize = len;
        sendRtpPacket(&mRtpPacket);
        mSeq++;

        /// 如果是VPS、SPS、PPS就不需要加时间戳
        if (nal_type == HEVC_NAL_VPS || nal_type == HEVC_NAL_SPS || nal_type == HEVC_NAL_PPS)
        {
            return;
        }
    }
    else 
    {
        /*
            create the HEVC payload header and transmit the buffer as fragmentation units (FU)

                0                   1
                0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
            +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            |F|   Type    |  LayerId  | TID |
            +-------------+-----------------+

                F       = 0
                Type    = 49 (fragmentation unit (FU))
                LayerId = 0
                TID     = 1
            */
        //分包先赋值RTP头三个字节
        //类型49表示该包是FU分包
        rtpHeader->payload[0] = 49 << 1;
        //保持原来H265类型字段第二个字段 一般都是1
        rtpHeader->payload[1] = 1;

        /*
                create the FU header

                0 1 2 3 4 5 6 7
                +-+-+-+-+-+-+-+-+
                |S|E|  FuType   |
                +---------------+

                S       = variable
                E       = variable
                FuType  = NAL unit type
            */
        //保持原来H265类型字段类型字段 
        rtpHeader->payload[2]  = nal_type;
        /* set the S bit: mark as start fragment */
        //设置开始标志
        rtpHeader->payload[2] |= 1 << 7;

        /* pass the original NAL header */
        //去掉H265 NAL头类型长度
        buf += 2;
        len -= 2;

        while (len > rtp_payload_size) {
            /* complete and send current RTP packet */
            memcpy(rtpHeader->payload+RTP_HEVC_HEADERS_SIZE, buf, rtp_payload_size);
            mRtpPacket.mSize = RTP_MAX_PKT_SIZE;
            sendRtpPacket(&mRtpPacket);

            mSeq++;

            buf += rtp_payload_size;
            len -= rtp_payload_size;

            /* reset the S bit */
            //去掉开始结束标志
            rtpHeader->payload[2] &= ~(1 << 7);
        }

        /* set the E bit: mark as last fragment */
        //设置结束标志
        rtpHeader->payload[2] |= 1 << 6;

        /* complete and send last RTP packet */
        memcpy(rtpHeader->payload+RTP_HEVC_HEADERS_SIZE, buf, len);
        mRtpPacket.mSize = len + RTP_HEVC_HEADERS_SIZE;
        sendRtpPacket(&mRtpPacket);
        mSeq++;
    }
    
    mTimestamp += mClockRate/mFps;
}

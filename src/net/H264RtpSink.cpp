#include <stdio.h>
#include <string.h>

#include "net/H264RtpSink.h"
#include "base/Logging.h"
#include "base/New.h"

H264RtpSink* H264RtpSink::createNew(MediaSource* mediaSource)
{
    if(!mediaSource)
        return NULL;

    return new H264RtpSink(mediaSource);
    // return New<H264RtpSink>::allocate(mediaSource);
}

H264RtpSink::H264RtpSink(MediaSource* mediaSource) :
    RtpSink(mediaSource, RTP_PAYLOAD_TYPE_H264),
    mClockRate(90000),
    mFps(mediaSource->getFps())
{
    start(1000/mFps);
}

H264RtpSink::~H264RtpSink()
{

}

std::string H264RtpSink::getMediaDescription(uint16_t port)
{
    char buf[100] = {0};
    sprintf(buf, "m=video %hu RTP/AVP %d", port, mPayloadType);

    return std::string(buf);
}

std::string H264RtpSink::getAttribute()
{
    char buf[100];
    sprintf(buf, "a=rtpmap:%d H264/%d\r\n", mPayloadType, mClockRate);
    sprintf(buf+strlen(buf), "a=framerate:%d", mFps);

    return std::string(buf);
}

void H264RtpSink::handleFrame(AVFrame* frame)
{
    
    RtpHeader* rtpHeader = mRtpPacket.mRtpHeadr;
    
    T_H264_NALU nalu = frame->getVideoFrame()->getNalu()->nalu.h264Nalu;

    const uint8_t *buf = nalu.buf + nalu.startcodeprefix_len;
    int len = nalu.len - nalu.startcodeprefix_len;

    uint8_t naluType = buf[0];
// printf("%s line=%d naluType=%d \n", __FUNCTION__, __LINE__, naluType & 0x1F);
    if(len <= RTP_MAX_PKT_SIZE)
    {
        memcpy(rtpHeader->payload, buf, len);
        mRtpPacket.mSize = len;
        sendRtpPacket(&mRtpPacket);
        mSeq++;

        if ((naluType & 0x1F) == 7 || (naluType & 0x1F) == 8) // 如果是SPS、PPS就不需要加时间戳
            return;
    }
    else
    {
        int pktNum = len / RTP_MAX_PKT_SIZE;       // 有几个完整的包
        int remainPktSize = len % RTP_MAX_PKT_SIZE; // 剩余不完整包的大小
        int i, pos = 1;

        /* 发送完整的包 */
        for (i = 0; i < pktNum; i++)
        {
            /*
            *     FU Indicator
            *    0 1 2 3 4 5 6 7
            *   +-+-+-+-+-+-+-+-+
            *   |F|NRI|  Type   |
            *   +---------------+
            * */
            rtpHeader->payload[0] = (naluType & 0x60) | 28; //(naluType & 0x60)表示nalu的重要性，28表示为分片
            
            /*
            *      FU Header
            *    0 1 2 3 4 5 6 7
            *   +-+-+-+-+-+-+-+-+
            *   |S|E|R|  Type   |
            *   +---------------+
            * */
            rtpHeader->payload[1] = naluType & 0x1F;
            
            if (i == 0) //第一包数据
                rtpHeader->payload[1] |= 0x80; // start
            else if (remainPktSize == 0 && i == pktNum - 1) //最后一包数据
                rtpHeader->payload[1] |= 0x40; // end

            memcpy(rtpHeader->payload+2, buf+pos, RTP_MAX_PKT_SIZE);
            mRtpPacket.mSize = RTP_MAX_PKT_SIZE+2;
            sendRtpPacket(&mRtpPacket);

            mSeq++;
            pos += RTP_MAX_PKT_SIZE;
        }

        /* 发送剩余的数据 */
        if (remainPktSize > 0)
        {
            rtpHeader->payload[0] = (naluType & 0x60) | 28;
            rtpHeader->payload[1] = naluType & 0x1F;
            rtpHeader->payload[1] |= 0x40; //end

            memcpy(rtpHeader->payload+2, buf+pos, remainPktSize);
            mRtpPacket.mSize = remainPktSize+2;
            sendRtpPacket(&mRtpPacket);

            mSeq++;
        }
    }
    
    mTimestamp += mClockRate/mFps;
}

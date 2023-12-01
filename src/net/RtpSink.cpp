#include <arpa/inet.h>

#include "net/RtpSink.h"
#include "base/Logging.h"
#include "base/New.h"

RtpSink::RtpSink(MediaSource* mediaSource, int payloadType) :
    mCsrcLen(0),
    mExtension(0),
    mPadding(0),
    mVersion(RTP_VESION),
    mPayloadType(payloadType),
    mMarker(0),
    mSeq(0),
    mTimestamp(0)
    
{
    mSSRC = rand();

    mMediaSource = mediaSource;
    mMediaSource->setEventHandle(this);
}

RtpSink::~RtpSink()
{

}

void RtpSink::sendRtpPacket(RtpPacket* packet)
{
    RtpHeader* rtpHead = packet->mRtpHeadr;
    rtpHead->csrcLen = mCsrcLen;
    rtpHead->extension = mExtension;
    rtpHead->padding = mPadding;
    rtpHead->version = mVersion;
    rtpHead->payloadType = mPayloadType;
    rtpHead->marker = mMarker;
    rtpHead->seq = htons(mSeq);
    rtpHead->timestamp = htonl(mTimestamp);
    rtpHead->ssrc = htonl(mSSRC);
    packet->mSize += RTP_HEADER_SIZE;
     
    if(m_sendframe_callback_func)
    {
        m_sendframe_callback_func(packet);
    }
        
}

void RtpSink::onFrameGetted(void *sender, std::shared_ptr<MediaSource::AVFrame> frame)
{
//  printf("%s line=%d \n", __FUNCTION__, __LINE__);
    if(frame == nullptr || frame.get() == nullptr)
    { 
        return;
    }
//  printf("%s line=%d \n", __FUNCTION__, __LINE__);
    handleFrame(frame.get());
}

void RtpSink::start(int ms)
{

}

void RtpSink::stop()
{

}
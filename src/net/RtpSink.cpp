#include <arpa/inet.h>

#include "net/RtpSink.h"
#include "base/Logging.h"
#include "base/New.h"

RtpSink::RtpSink(UsageEnvironment* env, MediaSource* mediaSource, int payloadType) :
    mMediaSource(mediaSource),
    mEnv(env),
    mCsrcLen(0),
    mExtension(0),
    mPadding(0),
    mVersion(RTP_VESION),
    mPayloadType(payloadType),
    mMarker(0),
    mSeq(0),
    mTimestamp(0),
    mTimerId(0)
    
{
    mTimerEvent = TimerEvent::createNew(this);
    mTimerEvent->setTimeoutCallback(timeoutCallback);

    mSSRC = rand();
}

RtpSink::~RtpSink()
{
    mEnv->scheduler()->removeTimedEvent(mTimerId);
    delete mTimerEvent;
    // Delete::release(mTimerEvent);
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

// void RtpSink::timeoutCallback(void* arg)
// {
//     RtpSink* rtpSink = (RtpSink*)arg;

//     rtpSink->mMediaSource->readFrame(); //从文件读取流

//     std::shared_ptr<AVFrame> frame = rtpSink->mMediaSource->getFrame();
//     if(frame == nullptr || frame.get() == nullptr)
//     { 
//         return;
//     }
 
//     rtpSink->handleFrame(frame.get());
// }

void RtpSink::timeoutCallback(void* arg)
{
    RtpSink* rtpSink = (RtpSink*)arg;

    // rtpSink->mMediaSource->readFrame(); //从文件读取流
//  printf("%s line=%d \n", __FUNCTION__, __LINE__);
    std::shared_ptr<AVFrame> frame = rtpSink->mMediaSource->getFrame();
    if(frame == nullptr || frame.get() == nullptr)
    { 
        return;
    }
//  printf("%s line=%d \n", __FUNCTION__, __LINE__);
    rtpSink->handleFrame(frame.get());
}

void RtpSink::start(int ms)
{
    mTimerId = mEnv->scheduler()->addTimedEventRunEvery(mTimerEvent, ms);
}

void RtpSink::stop()
{
    mEnv->scheduler()->removeTimedEvent(mTimerId);
}
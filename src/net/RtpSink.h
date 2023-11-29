#ifndef _MEDIA_SINK_H_
#define _MEDIA_SINK_H_
#include <string>
#include <stdint.h>
#include <functional>

#include "net/MediaSource.h"
#include "net/Event.h"
#include "net/UsageEnvironment.h"
#include "net/Rtp.h"


class RtpSink
{
public:
    RtpSink(UsageEnvironment* env, MediaSource* mediaSource, int payloadType);
    virtual ~RtpSink();

    virtual std::string getMediaDescription(uint16_t port) = 0;
    virtual std::string getAttribute() = 0;

    void setSendFrameCallbackFunc(std::function<void (RtpPacket*)> func){m_sendframe_callback_func = func;}

protected:
    virtual void handleFrame(AVFrame* frame) = 0;
    void sendRtpPacket(RtpPacket* packet);
    void start(int ms);
    void stop();

private:
    static void timeoutCallback(void*);

protected:
    UsageEnvironment* mEnv;
    MediaSource* mMediaSource;

    std::function<void (RtpPacket*)> m_sendframe_callback_func = nullptr; //发送数据回调函数

    uint8_t mCsrcLen;
    uint8_t mExtension;
    uint8_t mPadding;
    uint8_t mVersion;
    uint8_t mPayloadType;
    uint8_t mMarker;
    uint16_t mSeq;
    uint32_t mTimestamp;
    uint32_t mSSRC;

private:
    TimerEvent* mTimerEvent;
    Timer::TimerId mTimerId;
};

#endif //_MEDIA_SINK_H_
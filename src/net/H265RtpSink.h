#ifndef _H265_MEDIA_SINK_H_
#define _H265_MEDIA_SINK_H_
#include <stdint.h>

#include "net/RtpSink.h"

class H265RtpSink : public RtpSink
{
public:
    static H265RtpSink* createNew(MediaSource* mediaSource);
    
    H265RtpSink(MediaSource* mediaSource);
    virtual ~H265RtpSink();

    virtual std::string getMediaDescription(uint16_t port);
    virtual std::string getAttribute();
    virtual void handleFrame(AVFrame* frame);

private:
    RtpPacket mRtpPacket;
    int mClockRate;
    int mFps;

};

#endif //_H265_MEDIA_SINK_H_
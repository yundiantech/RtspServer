#ifndef _RTSP_STREAM_MANAGER_H_
#define _RTSP_STREAM_MANAGER_H_
#include <string>
#include <stdint.h>
#include <functional>

#include "base/Logging.h"
#include "net/UsageEnvironment.h"
#include "base/ThreadPool.h"
#include "net/EventScheduler.h"
#include "net/Event.h"
#include "net/RtspServer.h"
#include "net/MediaSession.h"
#include "net/InetAddress.h"
#include "net/H264FileMediaSource.h"
#include "net/H265RtpSink.h"
#include "net/AACFileMediaSource.h"
#include "net/AACRtpSink.h"

class RtspStreamManager
{
public:
    RtspStreamManager();
    virtual ~RtspStreamManager();

    void start_server();

    // void input_video_buffer(int stream_index);
    // void input_audio_buffer(int stream_index);

private:

};

#endif //_RTSP_STREAM_MANAGER_H_
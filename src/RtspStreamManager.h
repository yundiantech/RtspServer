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
#include "mediasource/H264FileMediaSource.h"
#include "mediasource/AACFileMediaSource.h"
#include "net/AACRtpSink.h"

class RtspStreamManager
{
public:
    RtspStreamManager();
    virtual ~RtspStreamManager();

    bool start_server();

    ///输入视频数据
    void inputFrame(VideoEncodedFramePtr videoFrame);

    ///输入音频数据
    void inputFrame(AACFramePtr audioFrame);

private:
    MediaSource* m_video_source = nullptr;
    MediaSource* m_audio_source = nullptr;

};

#endif //_RTSP_STREAM_MANAGER_H_
#include "RtspStreamManager.h"

#include <iostream>

#include "net/H264RtpSink.h"
#include "net/H265RtpSink.h"

RtspStreamManager::RtspStreamManager()    
{
    
}

RtspStreamManager::~RtspStreamManager()
{

}

bool RtspStreamManager::start_server()
{
    //Logger::setLogFile("xxx.log");
    Logger::setLogLevel(Logger::LogWarning);

    EventScheduler* scheduler = EventScheduler::createNew(EventScheduler::POLLER_SELECT);
    UsageEnvironment* env = UsageEnvironment::createNew(scheduler);

    Ipv4Address ipAddr("0.0.0.0", 8554);
    RtspServer* server = RtspServer::createNew(env, ipAddr);
    MediaSession* session = MediaSession::createNew("live");
    m_video_source = MediaSource::createNew();
    m_video_source->setFps(26);
    // RtpSink* videoRtpSink = H264RtpSink::createNew(m_video_source);
    RtpSink* videoRtpSink = H265RtpSink::createNew(m_video_source);
    m_audio_source = MediaSource::createNew();
    m_audio_source->setFps(43);
    RtpSink* audioRtpSink = AACRtpSink::createNew(m_audio_source);
 
    session->addRtpSink(MediaSession::TrackId0, videoRtpSink);
    session->addRtpSink(MediaSession::TrackId1, audioRtpSink);
    //session->startMulticast(); //多播
 
    server->addMeidaSession(session);
    bool ret = server->start();
    if (ret)
    {
        std::cout<<"Play the media using the URL \""<<server->getUrl(session)<<"\""<<std::endl;
        env->scheduler()->loop();
    }
    else
    {
        std::cout<<"start rtsp server failed!"<<std::endl;
    }
    
    return ret;
}

void RtspStreamManager::inputFrame(VideoEncodedFramePtr videoFrame)
{
    if (m_video_source && videoFrame && videoFrame.get())
    {
        m_video_source->inputFrame(videoFrame);
    }
}

void RtspStreamManager::inputFrame(AACFramePtr audioFrame)
{
    if (m_audio_source && audioFrame && audioFrame.get())
    {
        m_audio_source->inputFrame(audioFrame);
    }
}

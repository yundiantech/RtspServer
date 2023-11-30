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

void RtspStreamManager::start_server()
{
    //Logger::setLogFile("xxx.log");
    Logger::setLogLevel(Logger::LogWarning);

    EventScheduler* scheduler = EventScheduler::createNew(EventScheduler::POLLER_SELECT);
    UsageEnvironment* env = UsageEnvironment::createNew(scheduler);

    Ipv4Address ipAddr("0.0.0.0", 8554);
    RtspServer* server = RtspServer::createNew(env, ipAddr);
    MediaSession* session = MediaSession::createNew("live");
    m_video_source = H264FileMediaSource::createNew(env);
    // RtpSink* videoRtpSink = H264RtpSink::createNew(env, m_video_source);
    RtpSink* videoRtpSink = H265RtpSink::createNew(env, m_video_source);
    m_audio_source = AACFileMeidaSource::createNew(env);
    RtpSink* audioRtpSink = AACRtpSink::createNew(env, m_audio_source);

    session->addRtpSink(MediaSession::TrackId0, videoRtpSink);
    session->addRtpSink(MediaSession::TrackId1, audioRtpSink);
    //session->startMulticast(); //多播
 
    server->addMeidaSession(session);
    server->start();

    std::cout<<"Play the media using the URL \""<<server->getUrl(session)<<"\""<<std::endl;

    env->scheduler()->loop();
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

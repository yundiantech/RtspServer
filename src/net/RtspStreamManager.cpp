#include "RtspStreamManager.h"

#include <iostream>

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
    ThreadPool* threadPool = ThreadPool::createNew(2);
    UsageEnvironment* env = UsageEnvironment::createNew(scheduler, threadPool);

    Ipv4Address ipAddr("0.0.0.0", 8554);
    RtspServer* server = RtspServer::createNew(env, ipAddr);
    MediaSession* session = MediaSession::createNew("live");
    MediaSource* videoSource = H264FileMediaSource::createNew(env, "test.h265");
    RtpSink* videoRtpSink = H265RtpSink::createNew(env, videoSource);
    MediaSource* audioSource = AACFileMeidaSource::createNew(env, "test.aac");
    RtpSink* audioRtpSink = AACRtpSink::createNew(env, audioSource);

    session->addRtpSink(MediaSession::TrackId0, videoRtpSink);
    session->addRtpSink(MediaSession::TrackId1, audioRtpSink);
    //session->startMulticast(); //多播

    server->addMeidaSession(session);
    server->start();

    std::cout<<"Play the media using the URL \""<<server->getUrl(session)<<"\""<<std::endl;

    env->scheduler()->loop();
}
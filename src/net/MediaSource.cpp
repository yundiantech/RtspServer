#include "MediaSource.h"
#include "base/Logging.h"
#include "base/New.h"

#include <thread>

MediaSource* MediaSource::createNew()
{
    return new MediaSource();
    // return New<MediaSource>::allocate();
}

MediaSource::MediaSource()
{
    std::thread([=]
    {
        handleFramesFunc();
    }).detach();
}

MediaSource::~MediaSource()
{

}

// void MediaSource::inputFrame(AVFrame* frame)
// {
//     std::lock_guard<std::mutex > lck(m_mutex_frames);
//     std::shared_ptr<AVFrame> frame = std::make_shared<AVFrame>(frame);
//     mAVFrameQueue.push(frame);
// }

void MediaSource::inputFrame(std::shared_ptr<AVFrame> frame)
{
    std::lock_guard<std::mutex> lck(m_mutex_frames);
    mAVFrameQueue.push(frame);
}

void MediaSource::inputFrame(VideoEncodedFramePtr videoFrame)
{
    std::lock_guard<std::mutex> lck(m_mutex_frames);

    std::shared_ptr<AVFrame> frame = std::make_shared<AVFrame>();
    frame->setFrame(videoFrame);
    mAVFrameQueue.push(frame);
    m_con_frames.notify_all();
}

void MediaSource::inputFrame(AACFramePtr audioFrame)
{
    std::lock_guard<std::mutex> lck(m_mutex_frames);
    std::shared_ptr<AVFrame> frame = std::make_shared<AVFrame>();
    frame->setFrame(audioFrame);
    mAVFrameQueue.push(frame);
    m_con_frames.notify_all();
}

std::shared_ptr<MediaSource::AVFrame> MediaSource::getFrame()
{
    std::unique_lock<std::mutex> lck(m_mutex_frames);
    std::shared_ptr<MediaSource::AVFrame> frame = nullptr;

    while (mAVFrameQueue.empty())
    {
        m_con_frames.wait(lck);
    }

    if(!mAVFrameQueue.empty())
    {
        frame = mAVFrameQueue.front();    
        mAVFrameQueue.pop();
    }

    return frame;
}

void MediaSource::handleFramesFunc()
{
    while (1)
    {
        std::shared_ptr<MediaSource::AVFrame> frame = getFrame();

        if(frame == nullptr || frame.get() == nullptr)
        { 
            // break;
        }
        else if (m_event_handle)
        {
            m_event_handle->onFrameGetted(this, frame);
        }
    }
}
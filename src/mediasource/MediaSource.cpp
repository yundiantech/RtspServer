#include "MediaSource.h"
#include "base/Logging.h"
#include "base/New.h"

MediaSource::MediaSource(UsageEnvironment* env) :
    mEnv(env)
{
    // mMutex = Mutex::createNew();
    // for(int i = 0; i < DEFAULT_FRAME_NUM; ++i)
    //     mAVFrameInputQueue.push(&mAVFrames[i]);
}

MediaSource::~MediaSource()
{
    // delete mMutex;
    // Delete::release(mMutex);
}

// void MediaSource::inputFrame(AVFrame* frame)
// {
//     std::lock_guard<std::mutex > lck(m_mutex_frames);
//     std::shared_ptr<AVFrame> frame = std::make_shared<AVFrame>(frame);
//     mAVFrameQueue.push(frame);
// }

void MediaSource::inputFrame(std::shared_ptr<AVFrame> frame)
{
    std::lock_guard<std::mutex > lck(m_mutex_frames);
    mAVFrameQueue.push(frame);
}

void MediaSource::inputFrame(VideoEncodedFramePtr videoFrame)
{
    std::lock_guard<std::mutex > lck(m_mutex_frames);

    std::shared_ptr<AVFrame> frame = std::make_shared<AVFrame>();
    frame->setFrame(videoFrame);
    mAVFrameQueue.push(frame);
}

void MediaSource::inputFrame(AACFramePtr audioFrame)
{
    std::lock_guard<std::mutex > lck(m_mutex_frames);
    std::shared_ptr<AVFrame> frame = std::make_shared<AVFrame>();
    frame->setFrame(audioFrame);
    mAVFrameQueue.push(frame);
}

std::shared_ptr<AVFrame> MediaSource::getFrame()
{
    std::lock_guard<std::mutex > lck(m_mutex_frames);
    std::shared_ptr<AVFrame> frame = nullptr;

    if(!mAVFrameQueue.empty())
    {
        frame = mAVFrameQueue.front();    
        mAVFrameQueue.pop();
    }

    return frame;
}

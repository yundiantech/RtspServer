#ifndef _MEDIA_SOURCE_H_
#define _MEDIA_SOURCE_H_
#include <queue>
#include <mutex>
#include <stdint.h>

#include "net/UsageEnvironment.h"
#include "base/Mutex.h"
#include "frame/AACFrame.h"
#include "frame/VideoEncodedFrame.h"

class AVFrame
{
public:
    enum AVFrameType
    {
        AVFrameType_VIDEO = 0,
        AVFrameType_AUDIO
    };

    AVFrame(){}

    ~AVFrame(){}

    void setFrame(VideoEncodedFramePtr frame)
    {
        mType = AVFrameType_VIDEO;
        mVideoFrame = frame;
    }

    void setFrame(AACFramePtr frame)
    {
        mType = AVFrameType_AUDIO;
        mAudioFrame = frame;
    }

    VideoEncodedFramePtr getVideoFrame(){return mVideoFrame;}
    AACFramePtr getAudioFrame(){return mAudioFrame;}

    // //这三个变量没用了，用到的地方要改掉
    // uint8_t* mBuffer;
    // uint8_t* mFrame;
    // int mFrameSize;

private:
    AVFrameType mType;
    VideoEncodedFramePtr mVideoFrame;
    AACFramePtr mAudioFrame;

};

class MediaSource
{
public:
    MediaSource(UsageEnvironment* env);
    virtual ~MediaSource();

    int getFps() const { return mFps; }

    void inputFrame(std::shared_ptr<AVFrame> frame);
    void inputFrame(VideoEncodedFramePtr videoFrame);
    void inputFrame(AACFramePtr audioFrame);
    std::shared_ptr<AVFrame> getFrame();
 
    void setFps(int fps) { mFps = fps; }

    // virtual void readFrame() = 0;
    
protected:
    UsageEnvironment* mEnv;
    // AVFrame mAVFrames[DEFAULT_FRAME_NUM];
    // std::queue<AVFrame*> mAVFrameInputQueue;
    // std::queue<AVFrame*> mAVFrameOutputQueue;
    // Mutex* mMutex;

    int mFps;
    std::mutex m_mutex_frames;
    std::queue<std::shared_ptr<AVFrame>> mAVFrameQueue;
};

#endif //_MEDIA_SOURCE_H_
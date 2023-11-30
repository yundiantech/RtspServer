#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "H264FileMediaSource.h"
#include "base/Logging.h"
#include "base/New.h"

// static inline int startCode3(uint8_t* buf);
// static inline int startCode4(uint8_t* buf);

H264FileMediaSource* H264FileMediaSource::createNew(UsageEnvironment* env, const std::string &file)
{
    return new H264FileMediaSource(env, file);
    // return New<H264FileMediaSource>::allocate(env, file);
}

H264FileMediaSource::H264FileMediaSource(UsageEnvironment* env, const std::string& file) :
    MediaSource(env)
    // , mFile(file)
{
    // mFd = ::open(file.c_str(), O_RDONLY);
    // assert(mFd > 0);

    setFps(25);

}

H264FileMediaSource::~H264FileMediaSource()
{
    // ::close(mFd);
}

// void H264FileMediaSource::readFrame()
// {
//     std::shared_ptr<AVFrame> frame = std::make_shared<AVFrame>();

// #define FRAME_MAX_SIZE 102400
//     uint8_t buffer[FRAME_MAX_SIZE] = {0};
//     int frameSize = getFrameFromH264File(mFd, buffer, FRAME_MAX_SIZE);
//     if(frameSize < 0)
//         return;

//     int start_code_size = 0;

//     if(startCode3(buffer))
//     {
//         start_code_size = 3;
//     }
//     else
//     {
//         start_code_size = 4;
//     }


//     VideoEncodedFramePtr videoFrame = std::make_shared<VideoEncodedFrame>();
//     videoFrame->setNalu(buffer + start_code_size, frameSize - start_code_size, true, T_NALU_H264);
//     // videoFrame->setIsKeyFrame(key_frame);

//     inputFrame(videoFrame);
// }

// static inline int startCode3(uint8_t* buf)
// {
//     if(buf[0] == 0 && buf[1] == 0 && buf[2] == 1)
//         return 1;
//     else
//         return 0;
// }

// static inline int startCode4(uint8_t* buf)
// {
//     if(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 1)
//         return 1;
//     else
//         return 0;
// }

// static uint8_t* findNextStartCode(uint8_t* buf, int len)
// {
//     int i;

//     if(len < 3)
//         return NULL;

//     for(i = 0; i < len-3; ++i)
//     {
//         if(startCode3(buf) || startCode4(buf))
//             return buf;
        
//         ++buf;
//     }

//     if(startCode3(buf))
//         return buf;

//     return NULL;
// }

// int H264FileMediaSource::getFrameFromH264File(int fd, uint8_t* frame, int size)
// {
//     int rSize, frameSize;
//     uint8_t* nextStartCode;

//     if(fd < 0)
//         return fd;

//     rSize = read(fd, frame, size);
//     if(!startCode3(frame) && !startCode4(frame))
//         return -1;
    
//     nextStartCode = findNextStartCode(frame+3, rSize-3);
//     if(!nextStartCode)
//     {
//         lseek(fd, 0, SEEK_SET);
//         frameSize = rSize;
//     }
//     else
//     {
//         frameSize = (nextStartCode-frame);
//         lseek(fd, frameSize-rSize, SEEK_CUR);
//     }

//     return frameSize;
// }
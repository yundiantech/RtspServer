#ifndef _H265FILE_MEDIA_SOURCE_H_
#define _H265FILE_MEDIA_SOURCE_H_
#include <string>

#include "net/UsageEnvironment.h"
#include "net/MediaSource.h"
#include "base/ThreadPool.h"

class H265FileMediaSource : public MediaSource
{
public:
    static H265FileMediaSource* createNew(UsageEnvironment* env, std::string file);
    
    H265FileMediaSource(UsageEnvironment* env, const std::string& file);
    ~H265FileMediaSource();

protected:
    virtual void readFrame();

private:
    int getFrameFromH264File(int fd, uint8_t* frame, int size);

private:
    std::string mFile;
    int mFd;
};

#endif //_H265FILE_MEDIA_SOURCE_H_
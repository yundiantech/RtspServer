#ifndef VIDEOENCODEDFRAME_H
#define VIDEOENCODEDFRAME_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <memory>

#include "nalu/nalu.h"

#define VideoEncodedFramePtr std::shared_ptr<VideoEncodedFrame>

class VideoEncodedFrame
{
public:
    VideoEncodedFrame();
    ~VideoEncodedFrame();

    void setNalu(T_NALU *nalu, const int64_t &time = 0);
    void setNalu(uint8_t *buffer, const int &len, const bool & isAllocBuffer, const T_NALU_TYPE &type, const int64_t &time = 0);

    void setIsKeyFrame(const bool &isKeyFrame){mIsKeyFrame = isKeyFrame;}

    T_NALU *getNalu(){return mNalu;}
    bool getIsKeyFrame(){return mIsKeyFrame;}
    int64_t getPts(){return mPts;}

    uint8_t *getBuffer();
    unsigned int getSize();

private:
    T_NALU *mNalu;

    bool mIsKeyFrame;

    int64_t mPts;
};

#endif // VIDEOFRAME_H

#ifndef AACREADER_H
#define AACREADER_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "frame/AACFrame.h"

class AACReader
{
public:
    AACReader();

    int inputAACData(uint8_t *buf, int len); //输入aac数据

    ///从AAC数据中查找出一帧数据
    AACFramePtr getNextFrame();

private:
    uint8_t *mAACBuffer;
    int mBufferSize;

    ///读取adts头
    bool ReadAdtsHeader(unsigned char * adts_headerbuf, ADTS_HEADER *adts);

};

#endif // AACREADER_H

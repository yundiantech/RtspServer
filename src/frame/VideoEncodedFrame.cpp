﻿#include "VideoEncodedFrame.h"

VideoEncodedFrame::VideoEncodedFrame()
{
    mNalu = nullptr;
    mPts = 0;
    mIsKeyFrame = false;
}

VideoEncodedFrame::~VideoEncodedFrame()
{
    NALUParsing::FreeNALU(mNalu); //释放NALU内存
    mNalu = nullptr;
}

void VideoEncodedFrame::setNalu(T_NALU *nalu, const int64_t &time)
{
    mNalu = nalu;
    mPts  = time;
}

void VideoEncodedFrame::setNalu(uint8_t *buffer, const int &len, const bool &isAllocBuffer, const T_NALU_TYPE &type, const int64_t &time)
{
    T_NALU *nalu = NALUParsing::AllocNALU(len, type, isAllocBuffer);

    nalu->type = type;

    if (type == T_NALU_H264)
    {
        T_H264_NALU_HEADER *nalu_header = (T_H264_NALU_HEADER *)(buffer);

        nalu->nalu.h264Nalu.startcodeprefix_len = 4;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
//        nalu->nalu.h264Nalu.len = len;                 //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
        nalu->nalu.h264Nalu.forbidden_bit = 0;            //! should be always FALSE
        nalu->nalu.h264Nalu.nal_reference_idc = nalu_header->NRI;        //! NALU_PRIORITY_xxxx
        nalu->nalu.h264Nalu.nal_unit_type = nalu_header->TYPE;            //! NALU_TYPE_xxxx
        nalu->nalu.h264Nalu.lost_packets = false;  //! true, if packet loss is detected

        if (isAllocBuffer)
        {
            memcpy(nalu->nalu.h264Nalu.buf, buffer, len);  //! contains the first byte followed by the EBSP
        }
        else
        {
            nalu->nalu.h264Nalu.buf = buffer;
        }
        nalu->nalu.h264Nalu.max_size = len;
        nalu->nalu.h264Nalu.len = len;

//        {
//            char *bufTmp = (char*)(Buf + StartCode);
//            char s[10];
//            itoa(bufTmp[0], s, 2);
//            fprintf(stderr, "%s %08s %x %d\n", __FUNCTION__, s, bufTmp[0] , nalu_header->TYPE);
//        }
    }
    else
    {
        T_H265_NALU_HEADER *nalu_header = (T_H265_NALU_HEADER *)buffer;

        nalu->nalu.h265Nalu.startCodeLen = 4;      //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
        nalu->nalu.h265Nalu.h265NaluHeader = *nalu_header;

        if (isAllocBuffer)
        {
            memcpy(nalu->nalu.h265Nalu.buf, buffer, len);  //! contains the first byte followed by the EBSP
        }
        else
        {
            nalu->nalu.h265Nalu.buf = buffer;
        }

//        nalu->nalu.h265Nalu.max_size = len;
        nalu->nalu.h265Nalu.len = len;

//        {
//            char *bufTmp = (char*)(buffer);
//            fprintf(stderr, "%s %02x%02x%02x%02x%02x%02x %d %d\n", __FUNCTION__, bufTmp[0], bufTmp[1], bufTmp[2], bufTmp[3], bufTmp[4], bufTmp[5], nalu->nalu.h265Nalu.h265NaluHeader.nal_unit_type, nalu_header->nal_unit_type);
//        }
    }

    if (mNalu != nullptr)
    {
        NALUParsing::FreeNALU(mNalu); //释放NALU内存
    }

    setNalu(nalu, time);
}

uint8_t *VideoEncodedFrame::getBuffer()
{
    uint8_t *buffer = nullptr;

    do{
        if (!mNalu)
        {
            break;
        }

        if (mNalu->type == T_NALU_H264)
        {
            buffer = mNalu->nalu.h264Nalu.buf;
        }
        else if (mNalu->type == T_NALU_H265)
        {
            buffer = mNalu->nalu.h265Nalu.buf;
        }

    } while (0);

    return buffer;
}

unsigned int VideoEncodedFrame::getSize()
{
    unsigned int buffer_size = 0;

    do{
        if (!mNalu)
        {
            break;
        }

        if (mNalu->type == T_NALU_H264)
        {
            buffer_size = mNalu->nalu.h264Nalu.len;
        }
        else if (mNalu->type == T_NALU_H265)
        {
            buffer_size = mNalu->nalu.h265Nalu.len;
        }

    } while (0);

    return buffer_size;
}
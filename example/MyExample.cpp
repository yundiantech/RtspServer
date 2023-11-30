#include <iostream>

#include "RtspStreamManager.h"
#include "mediasource/h264/h264decoder.h"
#include "mediasource/aac/AACReader.h"

#include <thread>

#define VIDEO_SAMPLE_RATE 90000
#define AUDIO_FRAME_SIZE 320
#define AUDIO_SAMPLE_RATE 8000

uint64_t audioPts = 0;

// //输出NALU长度和TYPE
// void dump(T_NALU *n)
// {
//     if (!n)return;

//     printf(" len: %d nal_unit_type: %x\n", n->len, n->nal_unit_type);
// }

void readH265File(RtspStreamManager *stream_manager);
void readH264File(RtspStreamManager *stream_manager);
void readAACFile(RtspStreamManager *stream_manager);

static void mSleep(int mSecond)
{
#if defined(WIN32)
    Sleep(mSecond);
#else
    usleep(mSecond * 1000);
#endif
}

int main(int argc, char* argv[])
{
    RtspStreamManager *stream_manager = new RtspStreamManager();

    std::thread([=]
    {
        readH265File(stream_manager);
        
    }).detach();

    // std::thread([=]
    // {
    //     readH264File(stream_manager);
        
    // }).detach();

    std::thread([=]
    {
        readAACFile(stream_manager);
        
    }).detach();

    stream_manager->start_server();

    printf("%s line=%d \n", __FUNCTION__, __LINE__);
    return 0;
}

void readH265File(RtspStreamManager *stream_manager)
{
    FILE *fp = fopen("./test.h265", "rb");
    if (fp == NULL)
    {
        printf("H265 file not exist! \n");
        exit(-1);
    }

    int frameNum = 0; //当前播放的帧序号
    int frameRate = 0; //帧率

    NALUParsing naluParser;

    while(1)
    {
        char buf[10240] = {0};
        int size = fread(buf, 1, 1024, fp);//从h265文件读1024个字节 (模拟从网络收到h265流)

        if (size <= 0)
        {
            printf("readH265File finished! \n");

            fseek(fp, 0, SEEK_SET);
            mSleep(1000);
            continue;
        }

        int nCount = naluParser.inputData(T_NALU_H265, (uint8_t*)buf, size, false);
        
// printf("%s line=%d %d\n", __FUNCTION__, __LINE__, nCount);
        while(1)
        {
            //从前面读到的数据中获取一个nalu
            T_NALU* nalu = naluParser.getNextFrame();
            if (nalu == NULL) break;

//            dump(nalu);
// printf("%s line=%d %d %d\n", __FUNCTION__, __LINE__, frameRate, nalu->nalu.h264Nalu.nal_unit_type);
            ///计算帧率 
            if (frameRate <= 0)
            {
                /// h265从帧中解析帧率 暂未实现，直接赋值25
                frameRate = 25;
            }
// printf("%s line=%d %d\n", __FUNCTION__, __LINE__, frameRate);
            ///统计帧数
            {
                uint8_t type = 0;

                int len = nalu->nalu.h265Nalu.len;

                len -= nalu->nalu.h265Nalu.startCodeLen;

                int nal_type = (nalu->nalu.h265Nalu.buf[0] >> 1) & 0x3F;
                /// VPS、SPS、PPS 不计入帧数统计，不用于计算pts
                if (nal_type == HEVC_NAL_VPS || nal_type == HEVC_NAL_SPS || nal_type == HEVC_NAL_PPS)
                {

                } 
                else
                {
                    frameNum++;
                }
            }

            if (frameRate <= 0)
            {
                frameRate = 25;
            }

            /// h264裸数据不包含时间戳信息  因此只能根据帧率做同步
            /// 需要成功解码一帧后 才能获取到帧率
            uint64_t videoPts = (1000000 / frameRate * frameNum); //微秒
// printf("%s line=%d %d\n", __FUNCTION__, __LINE__, videoPts);
            ///根据音频时间戳做同步。
            // while (videoPts > audioPts || audioPts <= 0)
            // {
            //     usleep(5000);
            // }
            usleep(1000000 / frameRate);
// printf("%s line=%d \n", __FUNCTION__, __LINE__);
            {
                VideoEncodedFramePtr videoFrame = std::make_shared<VideoEncodedFrame>();
                videoFrame->setNalu(nalu);
                // videoFrame->setIsKeyFrame(key_frame);
                stream_manager->inputFrame(videoFrame);
            }
        }
    }
}

void readH264File(RtspStreamManager *stream_manager)
{
    FILE *fp = fopen("./test.h264", "rb");
//    FILE *fp = fopen("E:/out.h264", "rb");frameRate
    if (fp == NULL)
    {
        printf("H264 file not exist! \n");
        exit(-1);
    }

    int frameNum = 0; //当前播放的帧序号
    int frameRate = 0; //帧率

    NALUParsing naluParser;

    while(1)
    {
        char buf[10240] = {0};
        int size = fread(buf, 1, 1024, fp);//从h264文件读1024个字节 (模拟从网络收到h264流)

        if (size <= 0)
        {
            printf("readH264File finished! \n");

            fseek(fp, 0, SEEK_SET);
            mSleep(1000);
            continue;
        }

        int nCount = naluParser.inputData(T_NALU_H264, (uint8_t*)buf, size, false);
// printf("%s line=%d %d\n", __FUNCTION__, __LINE__, nCount);
        while(1)
        {
            //从前面读到的数据中获取一个nalu
            T_NALU* nalu = naluParser.getNextFrame();
            if (nalu == NULL) break;

//            dump(nalu);
// printf("%s line=%d %d %d\n", __FUNCTION__, __LINE__, frameRate, nalu->nalu.h264Nalu.nal_unit_type);
            ///计算帧率 
            if (frameRate <= 0)
            {
                uint8_t type = 0;

                int len = nalu->nalu.h264Nalu.len;

                type = nalu->nalu.h264Nalu.nal_unit_type;
                len -= nalu->nalu.h264Nalu.startcodeprefix_len;

                if (len < 1)
                    return;

                if (type == 7)
                {
                    uint8_t h264_sps[64] = {0};
                    printf("sps %d\n", len);
                    if (len > sizeof(h264_sps))
                        len = sizeof(h264_sps);
                    memcpy(h264_sps, nalu->nalu.h264Nalu.buf, len);
                    int h264_sps_len = len;

                    int width  = 0;
                    int height = 0;
                    int fps    = 0;

                    ///解析SPS信息，从中获取视频宽高和帧率
                    bool ret = H264Decoder::decodeSps(h264_sps, h264_sps_len, width, height, fps);

                    printf("ret = %d width = %d height = %d fps = %d\n", ret, width, height, fps);

                    if (fps <= 0) //解析后帧率为0 说明sps中不带帧率信息 这里给他设置成25
                    {
                        fps = 25;
                    }

                    frameRate = fps;
                }

                if (type == 8)
                {
                    printf("pps %d\n", len);
                }

            }
// printf("%s line=%d %d\n", __FUNCTION__, __LINE__, frameRate);
            ///统计帧数
            {
                uint8_t type = 0;

                int len = nalu->nalu.h264Nalu.len;

                type = nalu->nalu.h264Nalu.buf[0] & 0x1f;
                len -= nalu->nalu.h264Nalu.forbidden_bit;

                ///sps不计入帧数统计，不用于计算pts
                if (type == 7)
                {
//                    printf("is sps len=%d\n", len);
                }
                else if (type == 8)
                {
//                    printf("is pps len=%d\n", len);
                    frameNum++;
                }
                else
                {
                    frameNum++;
                }
            }

            if (frameRate <= 0)
            {
                frameRate = 25;
            }

            /// h264裸数据不包含时间戳信息  因此只能根据帧率做同步
            /// 需要成功解码一帧后 才能获取到帧率
            uint64_t videoPts = (1000000 / frameRate * frameNum); //微秒
// printf("%s line=%d %d\n", __FUNCTION__, __LINE__, videoPts);
            ///根据音频时间戳做同步。
            // while (videoPts > audioPts || audioPts <= 0)
            // {
            //     usleep(5000);
            // }
            usleep(1000000 / frameRate);
// printf("%s line=%d \n", __FUNCTION__, __LINE__);
            {
                VideoEncodedFramePtr videoFrame = std::make_shared<VideoEncodedFrame>();
                videoFrame->setNalu(nalu);
                // videoFrame->setIsKeyFrame(key_frame);
                stream_manager->inputFrame(videoFrame);
            }

            // NALUParsing::FreeNALU(nalu); //释放NALU内存
        }
    }
}

void readAACFile(RtspStreamManager *stream_manager)
{
    FILE *fp = fopen("./test.aac", "rb");
    if (fp == NULL)
    {
        printf("AAC file not exist!");
        exit(-1);
    }

    AACReader *mAACReader = new AACReader();

    while(1)
    {
        char buf[10240] = {0};
        int size = fread(buf, 1, 1024, fp);//从h264文件读1024个字节 (模拟从网络收到h264流)

        if (size <= 0)
        {
            printf("readAACFile finished! \n");

            fseek(fp, 0, SEEK_SET);
            mSleep(1000);
            continue;
        }

        int nCount = mAACReader->inputAACData((uint8_t*)buf, size);

        while(1)
        {
            //从前面读到的数据中获取一帧音频数据
            std::shared_ptr<AACFrame> aacFrame = mAACReader->getNextFrame();
            if (aacFrame == NULL) break;

//            audioPts += 1000000 / 43;
            audioPts += 6000;

            mSleep(6); 

            stream_manager->inputFrame(aacFrame);
        }
    }
}
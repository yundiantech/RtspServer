#ifndef H264DECODER_H
#define H264DECODER_H

typedef  unsigned int UINT;
typedef  unsigned char BYTE;
typedef  unsigned long DWORD;

class H264Decoder
{
public:
    H264Decoder();

    /**
     * 解码SPS,获取视频图像宽、高和帧率信息（次代码来源于网络，准确性没有得到验证）
     *
     * @param buf SPS数据内容
     * @param nLen SPS数据的长度
     * @param width 图像宽度
     * @param height 图像高度
     * @成功则返回true , 失败则返回false
     */
    static bool decodeSps(BYTE * buf,unsigned int nLen,int &width,int &height,int &fps);

};

#endif // H264DECODER_H

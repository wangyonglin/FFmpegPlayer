#include "FFmpegSpeaker.h"

#include <QAudioDeviceInfo>
#include <QDebug>

FFmpegSpeaker::FFmpegSpeaker(QObject *parent) : QObject(parent)
{

}
void FFmpegSpeaker::InitConfig(int dst_nb_samples, int rate, int sample_size, int nch)
{

    QAudioFormat format;
    format.setSampleRate(rate); // 采样率
    format.setChannelCount(nch);   // 声道数
    format.setSampleSize(sample_size);    // 采样大小
    format.setCodec("audio/pcm"); // 音频编码格式
    format.setByteOrder(QAudioFormat::LittleEndian); // 字节顺序
    format.setSampleType(QAudioFormat::SignedInt);  // 采样类型

    int len = dst_nb_samples * format.channelCount() * format.sampleSize()/8;
    Q_UNUSED(len);

    // 创建 QAudioOutput 对象
    audiooutput = new QAudioOutput(format);
    //audioOutput->setBufferSize(len * 100);
    audiooutput->setVolume(1.0); // 设置音量（0.0 - 1.0）

    // 打开音频输出
    io=audiooutput->start();
}

void FFmpegSpeaker::Player(const char *data, qint64 len)
{

    io->write(data, len);

}
void FFmpegSpeaker::Player(QByteArray bytes)
{

    if(io->isOpen() && !bytes.isEmpty()){
        io->write(bytes.data(), bytes.length());
    }

}
void FFmpegSpeaker::Stop()
{

    audiooutput->stop();
}




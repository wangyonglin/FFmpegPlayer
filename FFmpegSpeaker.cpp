#include "FFmpegSpeaker.h"
#include <QDebug>
#include <QAudioSink>

Qt6FFmpeg::FFmpegSpeaker::FFmpegSpeaker(QObject *parent)
    : QObject{parent}
{
    defaultDevice = QMediaDevices::defaultAudioOutput();
    defaultFormat.setSampleRate(16000);
    defaultFormat.setChannelCount(2);
    defaultFormat.setSampleFormat(QAudioFormat::Int16);
    qMediaDevices=new QMediaDevices(this);
    listDevices= qMediaDevices->audioOutputs();
    for (auto &audioDevice : listDevices){
        qInfo() << "可用扬声器:" <<     audioDevice.description();
    }

}


QList<QAudioDevice> Qt6FFmpeg::FFmpegSpeaker::getAudioDevices()
{
    return listDevices;
}

void Qt6FFmpeg::FFmpegSpeaker::init(const QAudioFormat &format,const QString &description)
{
    defaultFormat=format;

    for (auto &currentDevice : listDevices){
        if(currentDevice.description() == description){
            defaultDevice=currentDevice;
            break;
        }
    }
}

QAudioFormat Qt6FFmpeg::FFmpegSpeaker::format()
{
    return defaultFormat;
}

void Qt6FFmpeg::FFmpegSpeaker::destroy()
{
    if(qAudioSink){
        qAudioSink->stop();
        delete qAudioSink;
    }
}

void Qt6FFmpeg::FFmpegSpeaker::create()
{

    if (!defaultDevice.isFormatSupported(defaultFormat)) {
        qWarning() << "Default format not supported, trying to use the nearest.";
    }
    qAudioSink= new QAudioSink(defaultDevice, defaultFormat,this);
    qInfo() << tr("Speakers - %1").arg(defaultDevice.description());
    qIODevice=qAudioSink->start();


}

void Qt6FFmpeg::FFmpegSpeaker::write(const QByteArray &audio_bytes)
{
    if(!audio_bytes.isEmpty() && qIODevice){
        qIODevice->write(audio_bytes);
    }
}

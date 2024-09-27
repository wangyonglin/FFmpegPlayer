#ifndef FFMPEGSPEAKER_H
#define FFMPEGSPEAKER_H

#include <QObject>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QAudioSource>
#include <QAudioSink>
#include "Qt6FFmpeg_global.h"

namespace Qt6FFmpeg {

class QT6FFMPEG_EXPORT FFmpegSpeaker : public QObject
{
    Q_OBJECT
public:
    explicit FFmpegSpeaker(QObject *parent = nullptr);
    QList<QAudioDevice> getAudioDevices();
    QAudioFormat format();
    void destroy();
    void init(const QAudioFormat &format, const QString &description=nullptr);
    void create();
public slots:
    void write(const QByteArray &data);

private:
    QAudioDevice defaultDevice;
    QAudioFormat defaultFormat;
    QMediaDevices *qMediaDevices;
    QAudioSink* qAudioSink;
    QIODevice *qIODevice;
    QList<QAudioDevice> listDevices;
signals:
};
}
#endif // FFMPEGSPEAKER_H

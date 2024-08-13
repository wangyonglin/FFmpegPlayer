#ifndef FFmpegPacket_H
#define FFmpegPacket_H

#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>
#include <QThread>

#ifdef __cplusplus
extern "C"
{
#include "libavcodec/avcodec.h"
}
#endif


class FFmpegPacket : public QObject
{
    Q_OBJECT
public:
    explicit FFmpegPacket(QObject *parent = nullptr);
    void enqueue(AVPacket *packet);
    AVPacket* dequeue();
    void clear();
    int size();
    bool isEmpty();
    bool isFulled();

private:
    QQueue<AVPacket*> packetQueue;
    QMutex mutex;
    QWaitCondition waitCondition;
    int this_maxSize=100;
};

#endif // FFmpegPacket_H

#ifndef VideoDecoder_H
#define VideoDecoder_H

#include <QObject>
#include "FFmpegThreader.h"
#include "FFmpegManager.h"

class VideoDecoder : public FFmpegThreader
{
    Q_OBJECT
public:
    explicit VideoDecoder(QObject *parent = nullptr);
    bool frameFinished= true;
private:
    virtual void loopRunnable();
    FFmpegManager * manager;
    AVFrame         *yuvFrame     = NULL;
    void BuildDecoder(AVCodecContext *codec_ctx, FFmpegPacket *pkt_queue, FFmpegFrame *frame_queue);
    QImage frameToQImage(AVFrame *frame);
public slots:
    void freeParameters( FFmpegManager * controller);
    FFmpegManager *initParameters( FFmpegManager * controller);
signals:
    void drawImage(const QImage &image);
    void sigFirst(uchar* p,int w,int h);
    void newFrame();
    void updateImage(const QImage &image);
};

#endif // VideoDecoder_H

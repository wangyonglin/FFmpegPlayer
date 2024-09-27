#ifndef QFFMPEGPLAYER_H
#define QFFMPEGPLAYER_H

#include <QWidget>
#include "AudioDecoder.h"
#include "FFmpegDemuxer.h"
#include "VideoDecoder.h"
#include "Qt6FFmpeg_global.h"
namespace Qt6FFmpeg {
class QT6FFMPEG_EXPORT QFFmpegPlayer : public QWidget
{
    Q_OBJECT
public:

    explicit QFFmpegPlayer(QWidget *parent = nullptr);
    ~QFFmpegPlayer();

    void Play(const QString &url);
    void Stop();


private:
    FFmpegDemuxer *ffmpegDemuxer;
    AudioDecoder *audio_dec;
    VideoDecoder *video_dec;
    FFmpegManager * ffmpegManager;
    QImage frameImage;
public slots:
    void buildImage(const QImage &image);
signals:
    void finished();
    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;

    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *event) override;
};
}


#endif // QFFMPEGPLAYER_H

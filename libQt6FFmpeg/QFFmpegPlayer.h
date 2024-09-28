#ifndef QFFMPEGPLAYER_H
#define QFFMPEGPLAYER_H

#include <QWidget>
#include "AudioDecoder.h"
#include "FFmpegDemuxer.h"
#include "VideoDecoder.h"
#include "libQt6FFmpeg_global.h"
namespace Qt6FFmpeg {
class LIBQT6FFMPEG_EXPORT QFFmpegPlayer : public QWidget
{
    Q_OBJECT
public:

    explicit QFFmpegPlayer(QWidget *parent = nullptr);
    ~QFFmpegPlayer();

    void Play(const QString &url);
    void Stop();


private:
    FFmpegDemuxer *ffmpegDemuxer=nullptr;
    AudioDecoder *audio_dec=nullptr;
    VideoDecoder *video_dec=nullptr;
    FFmpegManager * ffmpegManager=nullptr;
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

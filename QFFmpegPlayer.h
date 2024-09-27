#ifndef FFMPEGPLAYER_H
#define FFMPEGPLAYER_H

#include <QWidget>
#include <QObject>
#include <QString>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include "AudioDecoder.h"
#include "FFmpegDemuxer.h"
#include "VideoDecoder.h"
#include "FFmpegPlayer_global.h"

class FFMPEGPLAYER_EXPORT FFmpegPlayer : public QWidget
{
    Q_OBJECT
public:

    explicit FFmpegPlayer(QWidget *parent = nullptr);
    ~FFmpegPlayer();

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

#endif // FFMPEGPLAYER_H

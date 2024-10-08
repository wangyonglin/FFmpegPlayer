#ifndef QFFMPEGPLAYER_H
#define QFFMPEGPLAYER_H

#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QOpenGLWidget>
#include "AudioDecoder.h"
#include "FFmpegDemuxer.h"
#include "VideoDecoder.h"
#include "FFmpegManager.h"
#include "libQt6FFmpeg_global.h"

namespace Qt6FFmpeg {
class LIBQT6FFMPEG_EXPORT QFFmpegPlayer : public QOpenGLWidget,public QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit QFFmpegPlayer(QWidget *parent = nullptr);
    ~QFFmpegPlayer();
    void play(const QString & url);
    void stop();


private:
    virtual void initializeGL();
    virtual void resizeGL(int w,int h);
    virtual void paintGL();
    FFmpegDemuxer *ffmpegDemuxer=nullptr;
    AudioDecoder *audio_dec=nullptr;
    VideoDecoder *video_dec=nullptr;
    FFmpegManager * ffmpegManager=nullptr;
    QOpenGLShaderProgram m_program;
    QOpenGLBuffer vbo;
    int idY,idU,idV;
    int width,height;
    uchar* ptr;
signals:
    void reject(int err);
    //void resolve();
public slots:
   // void resolveCallback();
    void rejectCallback(int err);
};

}


#endif // QFFMPEGPLAYER_H

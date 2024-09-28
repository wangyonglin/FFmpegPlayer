#ifndef FFMPEGPLAYER_H
#define FFMPEGPLAYER_H

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
class LIBQT6FFMPEG_EXPORT FFmpegPlayer : public QOpenGLWidget,public QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit FFmpegPlayer(QWidget *parent = nullptr);
    ~FFmpegPlayer();
    void play(const QString & url);
    void stop();
    void initializeGL();
    void resizeGL(int w,int h);
    void paintGL();

private:
    FFmpegDemuxer *ffmpegDemuxer;
    AudioDecoder *audio_dec;
    VideoDecoder *video_dec;
    FFmpegManager * ffmpegManager;
    //shader程序
    QOpenGLShaderProgram m_program;
    QOpenGLBuffer vbo;

    int idY,idU,idV;

    int width,height;

    //FFmpegDecoder *decoder;

    uchar* ptr;
signals:
    void finished();
};

}


#endif // FFMPEGPLAYER_H

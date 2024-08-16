#ifndef OpenGLFFmpegPlayer_H
#define OpenGLFFmpegPlayer_H

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
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include <QGLShaderProgram>
#include <mutex>

#include "AudioDecoder.h"
#include "FFmpegDemuxer.h"
#include "VideoDecoder.h"
#include "FFmpegManager.h"
#include "FFmpegPlayer_global.h"

class FFMPEGPLAYER_EXPORT OpenGLFFmpegPlayer : public QOpenGLWidget,public QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenGLFFmpegPlayer(QWidget *parent = nullptr);
    ~OpenGLFFmpegPlayer();
    void Play(const QString &url);
    void Stop();
private:
    FFmpegDemuxer *ffmpegDemuxer;
    AudioDecoder *audio_dec;
    VideoDecoder *video_dec;
    FFmpegManager * ffmpegManager;

public slots:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void buildImage(const QImage &image);
private:
    QVector<QVector3D> vertices;
    QVector<QVector2D> texCoords;
    QOpenGLShaderProgram *program;
    QOpenGLTexture *texture;
    QMatrix4x4 projection;

signals:
    void finished();
};

#endif // OpenGLFFmpegPlayer_H

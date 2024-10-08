#include "FFmpegPlayer.h"
#include <QDebug>


Qt6FFmpeg::FFmpegPlayer::FFmpegPlayer(QWidget *parent)
    :QOpenGLWidget(parent),
    ffmpegManager(new FFmpegManager(this)),
    ffmpegDemuxer(new FFmpegDemuxer(this)),
    audio_dec(new AudioDecoder(this)),
    video_dec(new VideoDecoder(this))
{
    connect(ffmpegDemuxer,&FFmpegDemuxer::demuxFinished,[=](){
        emit finished();
        qDebug("finished");
    });
    connect(video_dec,&VideoDecoder::sigFirst,[=](uchar* p,int w,int h){
        ptr = p;
        width = w;
        height = h;
    });
    connect(video_dec,&VideoDecoder::newFrame,[=](){
        update();
    });

    ffmpegDemuxer->start();
    audio_dec->start();
    video_dec->start();
}

Qt6FFmpeg::FFmpegPlayer::~FFmpegPlayer()
{
    ffmpegDemuxer->frameFinished=true;
    audio_dec->frameFinished=true;
    video_dec->frameFinished=true;
    ffmpegDemuxer->release(ffmpegManager);
    audio_dec->release(ffmpegManager);
    video_dec->release(ffmpegManager);
    ffmpegDemuxer->stop();
    audio_dec->stop();
    video_dec->stop();
}
void Qt6FFmpeg::FFmpegPlayer::play(const QString &url)
{
    ffmpegManager->url=url;
    ffmpegDemuxer->pause();
    audio_dec->pause();
    video_dec->pause();
    ffmpegDemuxer->frameFinished=true;
    audio_dec->frameFinished=true;
    video_dec->frameFinished=true;
    ffmpegManager->audio_pkt_queue->clear();
    ffmpegManager->audio_frame_queue->clear();
    ffmpegManager->video_pkt_queue->clear();
    ffmpegManager->video_frame_queue->clear();

    if((ffmpegDemuxer->init(ffmpegManager))){
        ffmpegManager->audio_stream_index=av_find_best_stream( ffmpegManager->ifmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
        ffmpegManager->video_stream_index=av_find_best_stream( ffmpegManager->ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
        if( ffmpegManager->audio_stream_index>=0){
            ffmpegManager->audio_codecpar= ffmpegManager->ifmt_ctx->streams[ ffmpegManager->audio_stream_index]->codecpar;
            ffmpegManager->audio_pts_base= ffmpegManager->ifmt_ctx->streams[ ffmpegManager->audio_stream_index]->time_base;
            ffmpegManager->audio_pts_begin =  ffmpegManager->ifmt_ctx->streams[ ffmpegManager->audio_stream_index]->start_time;
        }
        if( ffmpegManager->video_stream_index>=0)
        {
            ffmpegManager->video_codecpar= ffmpegManager->ifmt_ctx->streams[ ffmpegManager->video_stream_index]->codecpar;
            ffmpegManager->video_pts_base= ffmpegManager->ifmt_ctx->streams[ ffmpegManager->video_stream_index]->time_base;
            ffmpegManager->video_pts_begin =  ffmpegManager->ifmt_ctx->streams[ ffmpegManager->video_stream_index]->start_time;
        }

        audio_dec->init(ffmpegManager);
        video_dec->init(ffmpegManager);
        ffmpegDemuxer->frameFinished=false;
        audio_dec->frameFinished=false;
        video_dec->frameFinished=false;
        ffmpegManager->init_synchronize();
        ffmpegDemuxer->resume();
        audio_dec->resume();
        video_dec->resume();
    }

}

void Qt6FFmpeg::FFmpegPlayer::stop()
{
    ffmpegDemuxer->frameFinished=true;
    audio_dec->frameFinished=true;
    video_dec->frameFinished=true;

    ffmpegDemuxer->release(ffmpegManager);
    audio_dec->release(ffmpegManager);
    video_dec->release(ffmpegManager);

}

void Qt6FFmpeg::FFmpegPlayer::initializeGL()
{
    initializeOpenGLFunctions();
    const char *vsrc =
        "attribute vec4 vertexIn; \
        attribute vec4 textureIn; \
        varying vec4 textureOut;  \
        void main(void)           \
    {                         \
            gl_Position = vertexIn; \
            textureOut = textureIn; \
    }";

        const char *fsrc =
        "varying vec4 textureOut;\n"
        "uniform sampler2D textureY;\n"
        "uniform sampler2D textureU;\n"
        "uniform sampler2D textureV;\n"
        "void main(void)\n"
        "{\n"
        "vec3 yuv; \n"
        "vec3 rgb; \n"
        "yuv.x = texture2D(textureY, textureOut.st).r; \n"
        "yuv.y = texture2D(textureU, textureOut.st).r - 0.5; \n"
        "yuv.z = texture2D(textureV, textureOut.st).r - 0.5; \n"
        "rgb = mat3( 1,       1,         1, \n"
        "0,       -0.39465,  2.03211, \n"
        "1.13983, -0.58060,  0) * yuv; \n"
        "gl_FragColor = vec4(rgb, 1); \n"
        "}\n";

    m_program.addCacheableShaderFromSourceCode(QOpenGLShader::Vertex,vsrc);
    m_program.addCacheableShaderFromSourceCode(QOpenGLShader::Fragment,fsrc);
    m_program.link();

    GLfloat points[]{
        -1.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, -1.0f,
        -1.0f, -1.0f,

        0.0f,0.0f,
        1.0f,0.0f,
        1.0f,1.0f,
        0.0f,1.0f
    };

    vbo.create();
    vbo.bind();
    vbo.allocate(points,sizeof(points));

    GLuint ids[3];
    glGenTextures(3,ids);
    idY = ids[0];
    idU = ids[1];
    idV = ids[2];
}

void Qt6FFmpeg::FFmpegPlayer::resizeGL(int w, int h)
{
    if(h<=0) h=1;

    glViewport(0,0,w,h);
}

void Qt6FFmpeg::FFmpegPlayer::paintGL()
{
    if(!ptr) return;

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    m_program.bind();
    vbo.bind();
    m_program.enableAttributeArray("vertexIn");
    m_program.enableAttributeArray("textureIn");
    m_program.setAttributeBuffer("vertexIn",GL_FLOAT, 0, 2, 2*sizeof(GLfloat));
    m_program.setAttributeBuffer("textureIn",GL_FLOAT,2 * 4 * sizeof(GLfloat),2,2*sizeof(GLfloat));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,idY);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,width,height,0,GL_RED,GL_UNSIGNED_BYTE,ptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,idU);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,width >> 1,height >> 1,0,GL_RED,GL_UNSIGNED_BYTE,ptr + width*height);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,idV);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,width >> 1,height >> 1,0,GL_RED,GL_UNSIGNED_BYTE,ptr + width*height*5/4);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    m_program.setUniformValue("textureY",0);
    m_program.setUniformValue("textureU",1);
    m_program.setUniformValue("textureV",2);
    glDrawArrays(GL_QUADS,0,4);
    m_program.disableAttributeArray("vertexIn");
    m_program.disableAttributeArray("textureIn");
    m_program.release();
}

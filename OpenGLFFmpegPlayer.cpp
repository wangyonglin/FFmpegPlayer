#include "OpenGLFFmpegPlayer.h"


OpenGLFFmpegPlayer::OpenGLFFmpegPlayer(QWidget *parent)
    :QOpenGLWidget{parent},
      ffmpegManager(new FFmpegManager(this)),
      ffmpegDemuxer(new FFmpegDemuxer(this)),
      audio_dec(new AudioDecoder(this)),
      video_dec(new VideoDecoder(this))
{
    connect(ffmpegDemuxer,&FFmpegDemuxer::demuxFinished,[=](){
        emit finished();
        qDebug("finished");
    });

    connect(video_dec,&VideoDecoder::updateImage,this,&OpenGLFFmpegPlayer::buildImage);
    ffmpegDemuxer->loopStart();
    audio_dec->loopStart();
    video_dec->loopStart();
}

OpenGLFFmpegPlayer::~OpenGLFFmpegPlayer()
{
    ffmpegDemuxer->frameFinished=true;
    audio_dec->frameFinished=true;
    video_dec->frameFinished=true;
    ffmpegDemuxer->freeParameters(ffmpegManager);
    audio_dec->freeParameters(ffmpegManager);
    video_dec->freeParameters(ffmpegManager);
    ffmpegDemuxer->loopStop();
    audio_dec->loopStop();
    video_dec->loopStop();
}



void OpenGLFFmpegPlayer::Play(const QString &url)
{
    ffmpegManager->url=url;
    ffmpegDemuxer->loopPause();
    audio_dec->loopPause();
    video_dec->loopPause();
    ffmpegDemuxer->frameFinished=true;
    audio_dec->frameFinished=true;
    video_dec->frameFinished=true;
    ffmpegManager->audio_pkt_queue->clear();
    ffmpegManager->audio_frame_queue->clear();
    ffmpegManager->video_pkt_queue->clear();
    ffmpegManager->video_frame_queue->clear();

    if((ffmpegDemuxer->initParameters(ffmpegManager))){
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

        audio_dec->initParameters(ffmpegManager);
        video_dec->initParameters(ffmpegManager);
        ffmpegDemuxer->frameFinished=false;
        audio_dec->frameFinished=false;
        video_dec->frameFinished=false;
        ffmpegManager->init_synchronize();
        ffmpegDemuxer->loopResume();
        audio_dec->loopResume();
        video_dec->loopResume();
    }


}


void OpenGLFFmpegPlayer::Stop()
{

    ffmpegDemuxer->frameFinished=true;
    audio_dec->frameFinished=true;
    video_dec->frameFinished=true;

    ffmpegDemuxer->freeParameters(ffmpegManager);
    audio_dec->freeParameters(ffmpegManager);
    video_dec->freeParameters(ffmpegManager);

}





void OpenGLFFmpegPlayer::initializeGL()
{
    initializeOpenGLFunctions(); //初始化OPenGL功能函数
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);     //设置纹理2D功能可用

    program = new QOpenGLShaderProgram(this);
    if(!program->addShaderFromSourceFile(QOpenGLShader::Vertex,":/OpenGLFFmpegPlayer.vsh"))
        close();
    if(!program->addShaderFromSourceFile(QOpenGLShader::Fragment,":/OpenGLFFmpegPlayer.fsh")){
        close();
    }

    program->bindAttributeLocation("vertex", 0);//绑定顶点属性位置
    program->bindAttributeLocation("texCoord", 1);//绑定纹理属性位置
    // 链接着色器管道
    if (!program->link())
        close();
    // 绑定着色器管道
    if (!program->bind())
        close();
    //纹理坐标
    texCoords.append(QVector2D(0, 1)); //左上
    texCoords.append(QVector2D(1, 1)); //右上
    texCoords.append(QVector2D(0, 0)); //左下
    texCoords.append(QVector2D(1, 0)); //右下
    //顶点坐标
    vertices.append(QVector3D(-1, -1, 1));//左下
    vertices.append(QVector3D(1, -1, 1)); //右下
    vertices.append(QVector3D(-1, 1, 1)); //左上
    vertices.append(QVector3D(1, 1, 1));  //右上

    // 加载 Avengers.jpg 图片
    texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setFormat(QOpenGLTexture::RGBAFormat);
    //重复使用纹理坐标
    //纹理坐标(1.1, 1.2)与(0.1, 0.2)相同
    texture->setWrapMode(QOpenGLTexture::Repeat);
    //设置纹理大小
    texture->setSize(this->width(), this->height());

    //分配储存空间
    texture->allocateStorage();
}

void OpenGLFFmpegPlayer::resizeGL(int w, int h)
{
    // 计算窗口横纵比
    qreal aspect = qreal(w) / qreal(h ? h : 1);
    // 设置近平面值 3.0, 远平面值 7.0, 视场45度
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;
    // 重设投影
    projection.setToIdentity();
    // 设置透视投影
    projection.perspective(fov, static_cast<float>(aspect), zNear, zFar);
}

void OpenGLFFmpegPlayer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清除屏幕缓存和深度缓冲
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, -5.0);                   //矩阵变换


    program->setAttributeArray(0, vertices.constData());
    program->enableAttributeArray(0);
    program->setAttributeArray(1, texCoords.constData());
    program->enableAttributeArray(1);

    program->setUniformValue("texture", 0); //将当前上下文中位置的统一变量设置为value
    texture->bind();  //绑定纹理
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);//绘制纹理
    texture->release(); //释放绑定的纹理
    texture->destroy(); //消耗底层的纹理对象
    texture->create();
}

void OpenGLFFmpegPlayer::buildImage(const QImage &image)
{

    if(image.isNull())return;
     QImage lestImage=image.scaled(size(),Qt::KeepAspectRatio, Qt::SmoothTransformation);
    texture->setData(lestImage); //设置纹理图像
    //设置纹理细节
    texture->setLevelofDetailBias(-1);//值越小，图像越清晰
    update();
}

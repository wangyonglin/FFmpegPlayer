#include "QFFmpegPlayer.h"

#include <QPainter>
#include <QResizeEvent>

Qt6FFmpeg::QFFmpegPlayer::QFFmpegPlayer(QWidget *parent)
    :QWidget{parent},
    ffmpegManager(new FFmpegManager(this)),
    ffmpegDemuxer(new FFmpegDemuxer(this)),
    audio_dec(new AudioDecoder(this)),
    video_dec(new VideoDecoder(this))
{
    connect(ffmpegDemuxer,&FFmpegDemuxer::demuxFinished,[=](){
       emit finished();
       qDebug("finished");
    });
    connect(video_dec,&VideoDecoder::updateImage,this,&Qt6FFmpeg::QFFmpegPlayer::buildImage);

    ffmpegDemuxer->loopStart();
    audio_dec->loopStart();
    video_dec->loopStart();
}

Qt6FFmpeg::QFFmpegPlayer::~QFFmpegPlayer()
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



void Qt6FFmpeg::QFFmpegPlayer::Play(const QString &url)
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


void Qt6FFmpeg::QFFmpegPlayer::Stop()
{

    ffmpegDemuxer->frameFinished=true;
    audio_dec->frameFinished=true;
    video_dec->frameFinished=true;

    ffmpegDemuxer->freeParameters(ffmpegManager);
    audio_dec->freeParameters(ffmpegManager);
    video_dec->freeParameters(ffmpegManager);

}

void Qt6FFmpeg::QFFmpegPlayer::buildImage(const QImage &image)
{
    frameImage=image.scaled(size(),Qt::KeepAspectRatio, Qt::SmoothTransformation);

    update();
}

void Qt6FFmpeg::QFFmpegPlayer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawImage(rect(), frameImage);
}

void Qt6FFmpeg::QFFmpegPlayer::resizeEvent(QResizeEvent *event)
{
    resize(event->size());
}


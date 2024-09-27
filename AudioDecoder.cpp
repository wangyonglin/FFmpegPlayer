#include "AudioDecoder.h"
#include <memory>
#include <iostream>


AudioDecoder::AudioDecoder(QObject *parent)
    : FFmpegThreader(parent)
{
    ffmpegSpeaker= new  Qt6FFmpeg::FFmpegSpeaker(this);
    ffmpegResample = new FFmpegResampler(this);

}

AudioDecoder::~AudioDecoder()
{
}
void AudioDecoder::loopStart(Priority pri)
{
    FFmpegThreader::loopStart(pri);
}

void AudioDecoder::loopStop()
{
    FFmpegThreader::loopStop();
}
void AudioDecoder::loopPause()
{
    FFmpegThreader::loopPause();
}

void AudioDecoder::loopResume()
{
    FFmpegThreader::loopResume();
}

FFmpegManager *AudioDecoder::initParameters(FFmpegManager * manager){
    manager->audio_codec_ctx = avcodec_alloc_context3(NULL);
    int read_ret = avcodec_parameters_to_context(manager->audio_codec_ctx, manager->audio_codecpar);
    if(read_ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, read_ret);
        qDebug() << "avcodec_parameters_to_context failed" << errmsg;
        avcodec_free_context(&manager->audio_codec_ctx);
        return nullptr;
    }
    // h264
    // h264_qsv  AV_CODEC_ID_H264
    //    avcodec_find_decoder_by_name()

    //    if(AV_CODEC_ID_H264 == codec_ctx_->codec_id)
    //        codec = avcodec_find_decoder_by_name("h264_qsv");
    //    else
  const  AVCodec * codec = avcodec_find_decoder(manager->audio_codec_ctx->codec_id); //作业： 硬件解码
    if(!codec) {
        qDebug() << "avcodec_find_decoder failed";
        avcodec_free_context(&manager->audio_codec_ctx);
        return nullptr;
    }

    read_ret = avcodec_open2(manager->audio_codec_ctx, codec, NULL);
    if(read_ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, read_ret);
        qDebug() << "avcodec_open2 failed" << errmsg;
        avcodec_free_context(&manager->audio_codec_ctx);
        return nullptr;
    }
    this->manager=manager;

    ffmpegResample->InitFFmpegResampler(manager->audio_codec_ctx,AV_CH_LAYOUT_STEREO,44100,AV_SAMPLE_FMT_S16);
    int data_size = av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(2);
    format.setSampleFormat(QAudioFormat::SampleFormat::Int16);
    ffmpegSpeaker->init(format);
    ffmpegSpeaker->create();
    //ffmpegSpeaker->InitConfig(0,44100,data_size*8,2);
    return manager;
}

void AudioDecoder::freeParameters(FFmpegManager * manager){
    if(manager->audio_codec_ctx){
        avcodec_free_context(&manager->audio_codec_ctx);
        manager->audio_codec_ctx=nullptr;
    }
    ffmpegResample->FreeFFmpegResampler();
}


void AudioDecoder::loopRunnable()
{

    if(state()==Running && !frameFinished){
        if (manager->get_audio_synchronize() > manager->get_master_synchronize())
        {
            QThread::usleep(10);
            return;
        }
        if(!manager->audio_pkt_queue->isEmpty()){
            if(manager->audio_frame_queue->size() <10){
                BuildDecoder(manager->audio_codec_ctx,manager->audio_pkt_queue,manager->audio_frame_queue);
            }else{
                QThread::usleep(200);
                return;
            }
        }

        if(!manager->audio_frame_queue->isEmpty()){
            AVFrame * frame= manager->audio_frame_queue->dequeue();
            if(frame){
                int64_t pts_time= (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts;
                manager->audio_synchronize(pts_time,manager->audio_pts_begin,manager->audio_pts_base);
                QByteArray bytes= ffmpegResample->BuiledConvert(frame);
                ffmpegSpeaker->write(bytes);
                av_frame_free(&frame);
            }

        }
    }
}





void AudioDecoder::BuildDecoder(AVCodecContext *codec_ctx,FFmpegPacket *pkt_queue, FFmpegFrame *frame_queue)
{
    if(!codec_ctx)return;
    if(pkt_queue->isEmpty())return;
    AVPacket * pkt= pkt_queue->dequeue();
    if(!pkt)return;
    int ret = avcodec_send_packet(codec_ctx, pkt);
    av_packet_free(&pkt);
    if (ret < 0)
    {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, ret);
        qDebug() << "avcodec_send_packet failed" << errmsg;
        return ;
    }
    AVFrame* frame = av_frame_alloc();
    if(!frame)return;
    ret = avcodec_receive_frame(codec_ctx, frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
        return ;
    }else if (ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, ret);
        qDebug() << "avcodec_receive_frame failed" << errmsg;
        return ;
    }
    frame_queue->enqueue(frame);
    av_frame_free(&frame);
}




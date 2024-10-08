#include "QFFmpegTool.h"
#include <QDebug>
Qt6FFmpeg::QFFmpegTool::QFFmpegTool(QObject *parent)
    : QObject{parent}
{}

QImage Qt6FFmpeg::QFFmpegTool::frameToQImage(AVFrame *frame)
{

    if((!frame) || (frame->width<=0) || (frame->height<=0)){
        return QImage();
    }

    QImage img(frame->width, frame->height, QImage::Format_RGBA8888);

    SwsContext* sws_ctx = sws_getContext(frame->width,
                                         frame->height,
                                         static_cast<enum AVPixelFormat>(frame->format),
                                         frame->width,
                                         frame->height,
                                         AV_PIX_FMT_RGBA,
                                         SWS_BILINEAR,
                                         nullptr,
                                         nullptr,
                                         nullptr);


    if (!sws_ctx) {
        // 错误处理：sws_getContext 失败
        qDebug() << "sws_getContext failed.";
        return QImage();
    }

    uint8_t* data[1] = { reinterpret_cast<uint8_t*>(img.bits()) };
    int linesize[1] = { static_cast<int>(img.bytesPerLine()) };
    int ret = sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height, data, linesize);
    sws_freeContext(sws_ctx);
    if (ret != frame->height) {
        // 错误处理：sws_scale 失败
        qDebug() << "sws_scale failed.";
        return QImage();
    }
    return img;

}

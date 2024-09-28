#include "FFmpegManager.h"

FFmpegManager::FFmpegManager(QWidget *parent)
    : FFmpegSynchronizer{parent},

    audio_pkt_queue(new FFmpegPacket(this)),
    video_pkt_queue(new FFmpegPacket(this)),
    audio_frame_queue(new FFmpegFrame(this)),
    video_frame_queue(new FFmpegFrame(this))
{}

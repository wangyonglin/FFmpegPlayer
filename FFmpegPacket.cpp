#include "FFmpegPacket.h"

FFmpegPacket::FFmpegPacket(QObject *parent)
    : QObject{parent}
{}
void FFmpegPacket::enqueue(AVPacket *packet) {
    QMutexLocker locker(&mutex);
    if(packetQueue.size() <= this_maxSize){
        AVPacket *__packet = av_packet_alloc();
        av_packet_move_ref(__packet, packet);
        packetQueue.enqueue(__packet);
        waitCondition.wakeAll();
    }
}

AVPacket* FFmpegPacket::dequeue() {
    QMutexLocker locker(&mutex);
    while (packetQueue.isEmpty()) {
        waitCondition.wait(&mutex);

    }
    return packetQueue.dequeue();
}

void FFmpegPacket::clear() {
    QMutexLocker locker(&mutex);
    while (!packetQueue.isEmpty()) {
        AVPacket *frame = packetQueue.dequeue();
        if(frame){
            av_packet_free(&frame); // 释放帧内存
        }
    }
}

int FFmpegPacket::size()
{
    return packetQueue.size();
}

bool FFmpegPacket::isEmpty()
{
    return packetQueue.isEmpty();
}

bool FFmpegPacket::isFulled()
{
    if(!packetQueue.isEmpty()){
        if(packetQueue.size()>=this_maxSize){
            return true;
        }
    }
    return false;
}

#ifndef AudioDecoder_H
#define AudioDecoder_H
#include <QPointer>
#include "FFmpegThreader.h"
#include "FFmpegPacket.h"
#include "FFmpegFrame.h"
#include "FFmpegResampler.h"
#include "FFmpegManager.h"
#include "FFmpegSpeaker.h"

class AudioDecoder : public FFmpegThreader
{
    Q_OBJECT
public:
    explicit AudioDecoder(QObject *parent = nullptr);
    ~AudioDecoder();

   bool frameFinished= true;
private:
    virtual void loop();
    FFmpegResampler *ffmpegResample;
    Qt6FFmpeg::FFmpegSpeaker *ffmpegSpeaker;
    FFmpegManager * manager;
    void BuildDecoder(AVCodecContext *codec_ctx, FFmpegPacket *pkt_queue, FFmpegFrame *frame_queue);
public slots:
    virtual void start(Priority pri = InheritPriority);
    virtual void stop();
    virtual void pause();
    virtual void resume();
    void release(FFmpegManager * manager);
    FFmpegManager *init(FFmpegManager * manager);
};
#endif // AudioDecoder_H

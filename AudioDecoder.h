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
    virtual void loopRunnable();
    FFmpegResampler *ffmpegResample;
    FFmpegSpeaker *ffmpegSpeaker;
    FFmpegManager * manager;
    void BuildDecoder(AVCodecContext *codec_ctx, FFmpegPacket *pkt_queue, FFmpegFrame *frame_queue);
public slots:
    virtual void loopStart(Priority pri = InheritPriority);
    virtual void loopStop();
    virtual void loopPause();
    virtual void loopResume();
    void freeParameters(FFmpegManager * manager);
    FFmpegManager *initParameters(FFmpegManager * manager);
};
#endif // AudioDecoder_H

#ifndef FFmpegDemuxer_H
#define FFmpegDemuxer_H
#include "FFmpegThreader.h"
#include <QObject>
#include "FFmpegManager.h"

class FFmpegDemuxer : public FFmpegThreader
{
    Q_OBJECT
public:
    explicit FFmpegDemuxer(QObject *parent = nullptr);

public:
    bool frameFinished=true;
    FFmpegManager*manager;
public slots:
    virtual void loopStart(Priority pri = InheritPriority);
    virtual void loopStop();
    virtual void loopPause();
    virtual void loopResume();

    FFmpegManager* initParameters(FFmpegManager *manager);
    void freeParameters(FFmpegManager *manager);
private:
    virtual void loopRunnable() override;
signals:
    void demuxFinished();
};

#endif // FFmpegDemuxer_H

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
    virtual void start(Priority pri = InheritPriority);
    virtual void stop();
    virtual void pause();
    virtual void resume();

    FFmpegManager* init(FFmpegManager *manager);
    void release(FFmpegManager *manager);
private:
    virtual void loop() override;
signals:
    void reject(int err);
};

#endif // FFmpegDemuxer_H

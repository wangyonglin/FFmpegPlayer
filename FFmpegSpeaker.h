#ifndef FFMPEGSPEAKER_H
#define FFMPEGSPEAKER_H

#include <QObject>
#include <QAudioOutput>
#include <QIODevice>

class FFmpegSpeaker : public QObject
{
    Q_OBJECT
public:
    explicit FFmpegSpeaker(QObject *parent = nullptr);

    void InitConfig(int dst_nb_samples, int rate, int sample_size, int nch);
    void Player(const char* data, qint64 len);
    void Stop();
    void Player(QByteArray bytes);
private:
    QIODevice* io=nullptr;
    QAudioOutput* audiooutput=nullptr;
    int64_t player_start_time_ms = 0;

};

#endif // FFMPEGSPEAKER_H

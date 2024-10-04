#include "FFmpegThreader.h"

FFmpegThreader::FFmpegThreader(QObject *parent)
    : QThread(parent),
    pauseFlag(false),
    stopFlag(false)
{}

FFmpegThreader::~FFmpegThreader()
{

}

FFmpegThreader::State FFmpegThreader::state() const
{
    State s = Stoped;
    if (!QThread::isRunning())
    {
        s = Stoped;
    }
    else if (QThread::isRunning() && pauseFlag)
    {
        s = Paused;
    }
    else if (QThread::isRunning() && (!pauseFlag))
    {
        s = Running;
    }
    return s;
}

void FFmpegThreader::start(QThread::Priority pri)
{
    QThread::start(pri);
}



void FFmpegThreader::stop()
{
    if (QThread::isRunning())
    {
        stopFlag = true;
        condition.wakeAll();
        QThread::quit();
        QThread::wait();
    }
}

void FFmpegThreader::pause()
{
    if (QThread::isRunning())
    {
        pauseFlag = true;
    }
}

void FFmpegThreader::resume()
{
    if (QThread::isRunning())
    {
        pauseFlag = false;
        condition.wakeAll();
    }
}

void FFmpegThreader::run()
{
    //qDebug() << "enter thread : " << QThread::currentThreadId();
    while (!stopFlag)
    {

        loop(); // do something...

        if (pauseFlag)
        {
            mutex.lock();
            condition.wait(&mutex);
            mutex.unlock();
        }
    }
    pauseFlag = false;
    stopFlag = false;

    //qDebug() << "exit thread : " << QThread::currentThreadId();
}

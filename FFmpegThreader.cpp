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

void FFmpegThreader::loopStart(QThread::Priority pri)
{
    QThread::start(pri);
}



void FFmpegThreader::loopStop()
{
    if (QThread::isRunning())
    {
        stopFlag = true;
        condition.wakeAll();
        QThread::quit();
        QThread::wait();
    }
}

void FFmpegThreader::loopPause()
{
    if (QThread::isRunning())
    {
        pauseFlag = true;
    }
}

void FFmpegThreader::loopResume()
{
    if (QThread::isRunning())
    {
        pauseFlag = false;
        condition.wakeAll();
    }
}

void FFmpegThreader::run()
{
    qDebug() << "enter thread : " << QThread::currentThreadId();
    while (!stopFlag)
    {

        loopRunnable(); // do something...

        if (pauseFlag)
        {
            mutex.lock();
            condition.wait(&mutex);
            mutex.unlock();
        }
    }
    pauseFlag = false;
    stopFlag = false;

    qDebug() << "exit thread : " << QThread::currentThreadId();
}

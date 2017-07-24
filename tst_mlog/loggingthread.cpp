#include "loggingthread.h"

LoggingThread::LoggingThread()
    : QThread()
{
}

void LoggingThread::run()
{
    qDebug() << "Thread_start";
    qDebug() << "Thread_finish";
}

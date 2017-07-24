#ifndef LOGGINGTHREAD_H
#define LOGGINGTHREAD_H

#include <QThread>
#include <QDebug>

class LoggingThread : public QThread
{
public:
    LoggingThread();

private:
    void run();
};

#endif // LOGGINGTHREAD_H

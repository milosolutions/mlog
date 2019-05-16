/*******************************************************************************
Copyright (C) 2019 Milo Solutions
Contact: https://www.milosolutions.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QDebug>
#include <QTimer>

#include "exampleclass.h"
#include "mlog.h"

Q_LOGGING_CATEGORY(coreMain, "core.main")

//! Example use of MiloLog class
int main(int argc, char *argv[])
{
    // Create MiloLog instance before Q*Application to capture all messages it
    // generates
    MLog::instance();
    QCoreApplication a(argc, argv);
    a.setApplicationName("Basic example logger app");

    // Logger will log at most Info messages (qInfo, qCInfo).
    logger()->setLogLevel(MLog::InfoLog);

    // If you want to enable pushing all logs to a file (apart from console),
    // use enableLogToFile()
    logger()->enableLogToFile(a.applicationName());

    qCInfo(coreMain) << "Logger successfully created."
                      << "\n\tApplication name is:" << a.applicationName()
                      << "\n\tPrevious log path:" << logger()->previousLogPath()
                      << "\n\tCurrent log path:" << logger()->currentLogPath();

    qCWarning(coreMain) << "This is a warning!";
    qCCritical(coreMain) << "This is a critical message!";
    qCDebug(coreMain) << "This is a debug message, it won't be printed because"
                      << "log level is set to MLog::InfoLog";

    // Class test
    ExampleClass cls;
    // Note that you don't need to include milolog.h to use it!
    // (see logSomething() body)
    cls.logSomething();

    QTimer::singleShot(200, &a, &QCoreApplication::quit);
    return a.exec();
}

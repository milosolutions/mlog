/*******************************************************************************
  Copyright (C) 2016 Milo Solutions
  Contact: https://www.milosolutions.com

  This file is part of Milo Code Database, which is a collection of useful code
  snippets often reused within Milo Solutions. As such, this code is present in
  many projects done by Milo Solutions and does not constitute a copyright
  infridgement.

  You are entitled to do anything you please with this code, just as with the
  rest of code you bought. Files not containing this notice were written
  specifically for this project and have not been shared with any other party
  within or without Milo.
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

    // If you want to enable pushing all logs to a file (apart from console),
    // use enableLogToFile()
    logger()->enableLogToFile(a.applicationName());

    qCDebug(coreMain) << "Logger successfully created."
                      << "\n\tApplication name is:" << a.applicationName()
                      << "\n\tPrevious log path:" << logger()->previousLogPath()
                      << "\n\tCurrent log path:" << logger()->currentLogPath();

    // Class test
    ExampleClass cls;
    // Note that you don't need to include milolog.h to use it!
    // (see logSomething() body)
    cls.logSomething();

    QTimer::singleShot(1000, &a, &QCoreApplication::quit);
    return a.exec();
}

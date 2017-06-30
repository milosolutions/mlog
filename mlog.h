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

#pragma once

#include <QString>
#include <QFile>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(core)

class QMessageLogContext;

class MLog
{
public:
    static MLog *instance();
    void enableLogToFile(const QString &appName);
    void disableLogToFile();

    QString previousLogPath() const;
    QString currentLogPath() const;

private:
    Q_DISABLE_COPY(MLog)
    explicit MLog();
    ~MiloLog();
    static void messageHandler(QtMsgType type,
                               const QMessageLogContext &context,
                               const QString &message);
    void write(const QString &message);

    static MLog *_instance;
    QFile _logFile;
    QString _previousLogPath;
    QString _currentLogPath;
};

MiloLog *logger();

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

#pragma once

#include <QString>
#include <QMutex>
#include <QFile>
#include <QStandardPaths>
#include <QLoggingCategory>
#include <QDir>

Q_DECLARE_LOGGING_CATEGORY(core)

class QMessageLogContext;

class MLog
{
public:
    /*!
     * Determines which messages should be printed. See setLogLevel() for more
     * information.
     */
    enum LogLevel {
        NoLog, //! No log messages will be printed
        FatalLog, //! Only qFatal() messages will be printed
        CriticalLog, //! Only qFatal() and qCritical() messages will be printed
        WarningLog, //! Will print qFatal(), qCritical() and qWarning() messages
        InfoLog, //! Will print qFatal(), qCritical(), qWarning() and qInfo() messages
        DebugLog //! Will print all messages
    };

    /*!
     * Log file rotation. MLog saves log history from several runs of the
     * application.
     *
     * This is useful when debugging, user can run the application twice and
     * easily compare the logs from both runs.
     */
    enum class RotationType{
        Consequent, //! current -> previous -> previous-1 ...
        DateTime //! <appName>-<datetime>.log
    };
    
    static MLog *instance();
    void enableLogToFile(const QString &appName,
                         const QString &directory = QStandardPaths::writableLocation(
                             QStandardPaths::DocumentsLocation));
    void disableLogToFile();

    void setLogRotation(RotationType type, int maxLogs);

    void enableLogToConsole();
    void disableLogToConsole();

    QString previousLogPath() const;
    QString currentLogPath() const;

    void setLogLevel(const LogLevel level);
    LogLevel logLevel() const;

private:
    Q_DISABLE_COPY(MLog)
    explicit MLog();
    ~MLog();
    static void messageHandler(QtMsgType type,
                               const QMessageLogContext &context,
                               const QString &message);
    void write(const QString &message);
    bool isMessageAllowed(const QtMsgType qtLevel) const;
    void rotateLogFiles(const QString& appName);
    QString findPreviousLogPath(const QString &logFileDir, const QString &appName);
    void removeLastLog(const QString &appName, const QDir &logsDir);

    bool m_logToFile = false;
    bool m_logToConsole = true;
    QFile m_logFile;
    QString m_previousLogPath;
    QString m_currentLogPath;
    QMutex m_mutex;
    LogLevel m_logLevel = DebugLog;
    RotationType m_rotationType = RotationType::Consequent;
    int m_maxLogs = 2;
    const QString m_dateTimeFormat = QStringLiteral("yyyy-MM-dd_HH-mm-ss");
    const QString m_fileExt = QStringLiteral(".log");
};

MLog *logger();

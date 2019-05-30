/*******************************************************************************
Copyright (C) 2017 Milo Solutions
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

#include "mlog.h"

#include <QString>
#include <QTextStream>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QRegularExpression>
#include <QDateTime>

#ifdef ANDROID
#include "android/log.h"
#endif

Q_LOGGING_CATEGORY(coreLogger, "core.logger")

/*!
 * \class MLog
 * \brief Simple logger with capability to log into file
 *
 * This class is (sadly!) a singleton. If you call enableLogToFile(), MLog
 * will start writing logs (all uses of qDebug, qCDebug, qInfo, qWarning, etc.)
 * into both the console and a file.
 *
 * Location of the log file can be read using currentLogPath().
 *
 * Whenever enableLogToFile() is called, the old log (created in the previous
 * run of the application) is moved to previousLogPath(), and all subsequent
 * messages are saved to currentLogPath().
 *
 * To include MLog in your project, write this in your main routine:
  \code
  MLog::instance(); // This will setup the logger
  Q*Application a(argc, argv);
  logger()->enableLogToFile(a.applicationName()); // To enable writing to file
  \endcode
 *
 * You also need to include milolog.pri in your .pro file.
 *
 * You do not need to include milolog.h in files (other than main.cpp) - just
 * using qDebug and friends is enough.
 */


/*!
 * Installs Qt message handler. Sets up the default message pattern.
 *
 * If you are getting undefined method names in log output, add this to your
 * .pro file:
 \code
 DEFINES += QT_MESSAGELOGCONTEXT
 \endcode
 */
MLog::MLog()
{
    // use backslashes between '%' and '{' to avoid shadowing this placeholders with
    // similar placeholders from wizard.json file during the Qt Creator wizard creation
    qSetMessagePattern("%\{time}|%\{type}%\{if-category}|%\{category}%\{endif}|%\{function}: "
                       "%\{message}");
    qInstallMessageHandler(&messageHandler);
}

/*!
 * Just a destructor. Does not do anything interesting.
 */
MLog::~MLog()
{
}

/*!
 * Returns the instance of MLog (which is a singleton). You can also use the
 * logger() function to achieve the same goal.
 *
 * \sa logger()
 */
MLog *MLog::instance()
{
    static MLog sInstance;
    return &sInstance;
}

/*!
 * Creates a log file based on \a appName (application name) in \a directory and
 * opens it for writing. All future uses of qDebug, qCDebug, qInfo, etc. will be
 * printed to the console and written into that file.
 *
 * When this method is called, MLog saves log file created earlier (during
 * previous run of the application, or when enableLogToFile() was last run)
 * to previousLogPath().
 *
 * Log file path can be read using currentLogPath(). Previous log file is
 * available at previousLogPath().
 *
 * If you want to disable writing log to file while the application is still
 * running, use disableLogToFile().
 *
 * If you do not have logs \a directory it will be created.
 */
void MLog::enableLogToFile(const QString &appName, const QString &directory)
{
    // Check if logs directory exists and if does not exist try to create it
    QDir logsDir(directory);
    if (!logsDir.exists()) {
        qCDebug(coreLogger) << "Creating logs directory";
        if (logsDir.mkpath(directory)) {
            qCDebug(coreLogger) << "Directory was created successfully";
        } else {
            qCCritical(coreLogger) << "Could not create logs directory!";
            QCoreApplication::instance()->exit(1);
            return;
        }
    }

    m_previousLogPath = findPreviousLogPath(directory, appName);
    if (m_rotationType == MLog::RotationType::Consequent) {
        m_currentLogPath = directory + '/' + appName + "-current" + m_fileExt;
    }
    else if (m_rotationType == MLog::RotationType::DateTime) {
        const auto currentDate = QDateTime::currentDateTime().toString(m_dateTimeFormat);
        m_currentLogPath = directory + '/' + appName + "-" + currentDate + m_fileExt;
    }

    rotateLogFiles(appName);

    // Open appName-current.log and write init message
    m_logFile.setFileName(m_currentLogPath);
    if (!m_logFile.open(QFile::WriteOnly | QFile::Text)) {
        qCCritical(coreLogger) << "Could not open log file for writing!";
        QCoreApplication::instance()->exit(2);
        return;
    } else {
      m_logToFile = true;
    }
}

/*!
 * Disables writing logs into a file. Log messages will continue to be printed
 * into the console (cerr).
 */
void MLog::disableLogToFile()
{
    m_logFile.close();
    m_logToFile = false;
}

/*!
 * Sets log rotation to \a type.
 * \a maxLogs determines how many logs can be in directory
 * in format <appName>-<order_identifier>.log
 */
void MLog::setLogRotation(MLog::RotationType type, int maxLogs)
{
    m_rotationType = type;
    m_maxLogs = maxLogs;
}

/*!
 * Enables writing logs into a file. Log messages will continue to be printed
 * into the console (cerr).
 */
void MLog::enableLogToConsole()
{
    m_logToConsole = true;
}

/*!
 * Disables writing logs into a console. Log messages will continue to be printed
 * into the background (cerr).
 */
void MLog::disableLogToConsole()
{
    m_logToConsole = false;
}

/*!
 * Returns path where previous log file is saved. Previous log file was saved
 * after earlier run of enableLogToFile().
 *
 * Warning: this method will return an empty string if invoked *before*
 * enableLogToFile().
 */
QString MLog::previousLogPath() const
{
    return m_previousLogPath;
}

/*!
 * Returns path where current log file is saved. MLog will write all calls to
 * qDebug(), qCDebug(), qInfo(), etc. into this file.
 *
 * Warning: this method will return an empty string if invoked *before*
 * enableLogToFile().
 */
QString MLog::currentLogPath() const
{
    return m_currentLogPath;
}

/*!
 * Sets log level to \a level. Messages with value higher than \a level will not
 * be printed.
 *
 * For example, if \a level is set to MLog::WarningLog, MLog will not print
 * messages declared using qDebug() and qInfo(), but it will print qWarning(),
 * qCritical() and qFatal() messages.
 */
void MLog::setLogLevel(const MLog::LogLevel level)
{
    m_logLevel = level;
}

/*!
 * Returns current log level.
 *
 * Default value is MLog::DebugLog.
 */
MLog::LogLevel MLog::logLevel() const
{
    return m_logLevel;
}

/*!
 * Standard Qt messageHandler. MLog only adds the option to write to file
 * (use enableLogToFile() to, well, enable writing logs to a file) and log level
 * controls (see setLogLevel() and isMessageAllowed() for more details).
 *
 * \a type, \a context, \a message have exactly the same meaning as in Qt's
 * built-in message handler and behave the same.
 *
 * MLog respects categorized logging settings (qCDebug() and friends).
 *
 * \sa enableLogToFile, setLogLevel, isMessageAllowed
 */
void MLog::messageHandler(QtMsgType type, const QMessageLogContext &context,
                          const QString &message)
{
    MLog *log = logger();
    if (log->isMessageAllowed(type) == false)
        return;

    const QString formatted(qFormatLogMessage(type, context, message));
    if (log->m_logToFile)
      log->write(formatted + "\n");

    if (log->m_logToConsole == false)
      return;

#ifdef ANDROID
    android_LogPriority priority = ANDROID_LOG_DEBUG;
    switch (type) {
        case QtWarningMsg: priority = ANDROID_LOG_WARN; break;
        case QtCriticalMsg: priority = ANDROID_LOG_ERROR; break;
        case QtFatalMsg: priority = ANDROID_LOG_FATAL; break;
        case QtInfoMsg: priority = ANDROID_LOG_INFO; break;
        default: priority = ANDROID_LOG_DEBUG; break;
    };
    __android_log_print(priority, "Qt", "%s", qPrintable(formatted));
#else
    fprintf(stderr, "%s\n", qPrintable(formatted));
    fflush(stderr);
#endif
}

/*!
 * Writes \a message into current log file.
 *
 * This function first checks whether log file is open and writable.
 */
void MLog::write(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    if (m_logFile.isOpen() && m_logFile.isWritable()) {
        QTextStream logStream(&m_logFile);
        logStream.setCodec("UTF-8");
        logStream << message;
    }
}

/*!
 * Returns true if log level \a qtLevel is within logger logging level set
 * in setLogLevel().
 *
 * \sa setLogLevel
 */
bool MLog::isMessageAllowed(const QtMsgType qtLevel) const
{
    if (m_logLevel == NoLog)
        return false;

    switch (qtLevel) {
    case QtDebugMsg:
        return (m_logLevel >= DebugLog);
    case QtInfoMsg:
        return (m_logLevel >= InfoLog);
    case QtWarningMsg:
        return (m_logLevel >= WarningLog);
    case QtCriticalMsg:
        return (m_logLevel >= CriticalLog);
    case QtFatalMsg:
        return (m_logLevel >= FatalLog);
    }

    return false;
}

/*!
 * Returns the singleton instance of MLog. Synonym to MLog::instance(),
 * but easier to write.
 */
MLog *logger()
{
    return MLog::instance();
}

/*!
 * Rotates logFiles beginning with \a appName.
 *
 * This function changes file names when MLog::RotationType is Consequent.
 * It also deletes oldest log file if maxLogs is exceeded.
 *
 * \sa setLogRotation(RotationType type, int maxLogs)
 */
void MLog::rotateLogFiles(const QString &appName)
{
    const QString logFilePath(QStandardPaths::writableLocation(
                                  QStandardPaths::DocumentsLocation));
    const QDir logsDir(logFilePath);
    const QStringList logFilter(appName + "-*" + m_fileExt);
    const auto files = logsDir.entryList(logFilter, QDir::Files, QDir::Reversed);

    if (m_rotationType == MLog::RotationType::Consequent) {
        const QRegularExpression expr("("+appName+"-previous-)([1-9][0-9]*)"
                                      + m_fileExt);

        // replace names for all previous files
        for(const auto &file : qAsConst(files)) {
            const auto match = expr.match(file);
            if (match.hasMatch()) {
                const auto prefix = match.captured(1);
                int index = match.captured(2).toInt();
                ++index;

                QString newFilePath = prefix + QString::number(index) + m_fileExt;
                const auto oldFilePath = logsDir.absoluteFilePath(file);
                newFilePath = logsDir.absoluteFilePath(newFilePath);

                QFile::rename(oldFilePath, newFilePath);
            }
        }

        const QString newPrev = logFilePath + '/' + appName + "-previous-1" + m_fileExt;
        QFile::rename(m_previousLogPath, newPrev);
    }

    if (files.size()+1 > m_maxLogs)
        removeLastLog(appName, logsDir);

    if (QFileInfo(m_currentLogPath).exists())
        QFile::rename(m_currentLogPath, m_previousLogPath);
}

/*!
 * Finds previous log file path in \a logFileDir with \a appName.
 */
QString MLog::findPreviousLogPath(const QString &logFileDir, const QString &appName)
{
    if (m_rotationType == MLog::RotationType::Consequent) {
        return logFileDir + '/' + appName + "-previous" + m_fileExt;
    } else if (m_rotationType == MLog::RotationType::DateTime) {
        const QDir logsDir(logFileDir);
        const QStringList logFilter(appName + "-*" + m_fileExt);
        const auto files = logsDir.entryList(logFilter, QDir::Files, QDir::Reversed);
        const QRegularExpression expr(
            "(" + appName + "-)(\\d\\d\\d\\d-\\d\\d-\\d\\d_\\d\\d-\\d\\d-\\d\\d)"
            + m_fileExt);

        for(const auto &file : qAsConst(files)) {
            const auto match = expr.match(file);
            if (match.hasMatch())
                return logFileDir + '/' + file;
        }
    }
    return QString();
}

/*!
 * Removes last log file matching set RotationType in \a logsDir with \a appName.
 */
void MLog::removeLastLog(const QString &appName, const QDir &logsDir)
{
    const QStringList logFilter(appName + "-*" + m_fileExt);
    const auto files = logsDir.entryList(logFilter, QDir::Files, QDir::Reversed);
    const auto logFilePath = logsDir.absolutePath();
    QString lastLog;

    if (m_rotationType == MLog::RotationType::Consequent) {
        const QRegularExpression expr("(" + appName + "-previous-)([1-9][0-9]*)"
                                      + m_fileExt);
        int max = 0;

        for(const auto &file : qAsConst(files)) {
            const auto match = expr.match(file);
            if (match.hasMatch()) {
                const int index = match.captured(2).toInt();

                if (index > max) {
                    max = index;
                    lastLog = file;
                }
            }
        }
    } else if (m_rotationType == MLog::RotationType::DateTime) {
        const QRegularExpression expr("(" + appName + "-)"
                      "(\\d\\d\\d\\d-\\d\\d-\\d\\d_\\d\\d-\\d\\d-\\d\\d)"
                                + m_fileExt);
        QDateTime oldest = QDateTime::currentDateTime();

        for(const auto &file : qAsConst(files)) {
            const auto match = expr.match(file);
            if (match.hasMatch()) {
                const auto index = QDateTime::fromString(
                            match.captured(2), m_dateTimeFormat);

                if (index < oldest) {
                    oldest = index;
                    lastLog = file;
                }
            }
        }
    }

    QFile::remove(logFilePath + '/' + lastLog);
}

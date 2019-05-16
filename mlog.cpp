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

MLog *MLog::_instance = nullptr;

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
    if (!_instance) {
        _instance = new MLog();
    }

    return _instance;
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

    _previousLogPath = findPreviousLogPath(logFilePath,appName);
    if (_rotationType == MLog::RotationType::Consequent) {
        _currentLogPath = logFilePath + '/' + appName + "-current" + _fileExt;
    }
    else if (_rotationType == MLog::RotationType::DateTime) {
        const auto currentDate = QDateTime::currentDateTime().toString(_dateTimeFormat);
        _currentLogPath = logFilePath + '/' + appName + "-" + currentDate + _fileExt;
    }

    rotateLogFiles(appName);

    // Open appName-current.log and write init message
    _logFile.setFileName(_currentLogPath);
    if (!_logFile.open(QFile::WriteOnly | QFile::Text)) {
        qCCritical(coreLogger) << "Could not open log file for writing!";
        QCoreApplication::instance()->exit(2);
        return;
    } else {
      _logToFile = true;
    }
}

/*!
 * Disables writing logs into a file. Log messages will continue to be printed
 * into the console (cerr).
 */
void MLog::disableLogToFile()
{
    _logFile.close();
    _logToFile = false;
}

/*!
 * Sets log rotation to \a type.
 * \a maxLogs determines how many logs can be in directory
 * in format <appName>-<order_identifier>.log
 */
void MLog::setLogRotation(MLog::RotationType type, int maxLogs)
{
    _rotationType = type;
    _maxLogs = maxLogs;
}

/*!
 * Enables writing logs into a file. Log messages will continue to be printed
 * into the console (cerr).
 */
void MLog::enableLogToConsole()
{
    _logToConsole = true;
}

/*!
 * Disables writing logs into a console. Log messages will continue to be printed
 * into the background (cerr).
 */
void MLog::disableLogToConsole()
{
    _logToConsole = false;
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
    return _previousLogPath;
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
    return _currentLogPath;
}

void MLog::setLogLevel(const MLog::LogLevel level)
{
    _logLevel = level;
}

MLog::LogLevel MLog::logLevel() const
{
    return _logLevel;
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
    if (log->_logToFile)
      log->write(formatted + "\n");

    if (log->_logToConsole == false)
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
    QMutexLocker locker(&_mutex);
    if (_logFile.isOpen() && _logFile.isWritable()) {
        QTextStream logStream(&_logFile);
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
    if (_logLevel == NoLog)
        return false;

    switch (qtLevel) {
    case QtDebugMsg:
        return (_logLevel >= DebugLog);
    case QtInfoMsg:
        return (_logLevel >= InfoLog);
    case QtWarningMsg:
        return (_logLevel >= WarningLog);
    case QtCriticalMsg:
        return (_logLevel >= CriticalLog);
    case QtFatalMsg:
        return (_logLevel >= FatalLog);
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
    QDir logsDir(logFilePath);
    const QStringList logFilter(appName + "-*" + _fileExt);

    const auto files = logsDir.entryList(logFilter, QDir::Files, QDir::Reversed);

    if (_rotationType == MLog::RotationType::Consequent) {
        const QRegularExpression  expr("("+appName+"-previous-)([1-9][0-9]*)" + _fileExt);

        // replace names for all previous files
        for(auto file : qAsConst(files)) {
            const auto match = expr.match(file);
            if (match.hasMatch()) {
                const auto prefix = match.captured(1);
                int index = match.captured(2).toInt();
                ++index;

                QString newFilePath = prefix + QString::number(index) + _fileExt;
                const auto oldFilePath = logsDir.absoluteFilePath(file);
                newFilePath = logsDir.absoluteFilePath(newFilePath);

                QFile::rename(oldFilePath, newFilePath);
            }
        }
        const QString newPrev = logFilePath + '/' + appName + "-previous-1" + _fileExt;
        QFile::rename(_previousLogPath, newPrev);
    }

    if (files.size()+1 > _maxLogs)
        removeLastLog(appName, logsDir);

    if (QFileInfo(_currentLogPath).exists())
        QFile::rename(_currentLogPath, _previousLogPath);
}

/*!
 * Finds previous log file path in \a logFileDir with \a appName.
 */
QString MLog::findPreviousLogPath(const QString &logFileDir, const QString &appName)
{
    if (_rotationType == MLog::RotationType::Consequent) {
        return logFileDir + '/' + appName + "-previous" + _fileExt;
    }
    else if (_rotationType == MLog::RotationType::DateTime) {
        const QDir logsDir(logFileDir);
        const QStringList logFilter(appName + "-*" + _fileExt);
        const auto files = logsDir.entryList(logFilter, QDir::Files, QDir::Reversed);

        const QRegularExpression expr("(" + appName + "-)"
                      "(\\d\\d\\d\\d-\\d\\d-\\d\\d_\\d\\d-\\d\\d-\\d\\d)" + _fileExt);

        for(auto file : qAsConst(files)) {
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
    QStringList logFilter(appName + "-*" + _fileExt);
    const auto files = logsDir.entryList(logFilter, QDir::Files, QDir::Reversed);
    const auto logFilePath = logsDir.absolutePath();
    QString lastLog;

    if (_rotationType == MLog::RotationType::Consequent) {
        const QRegularExpression expr("(" + appName + "-previous-)([1-9][0-9]*)" + _fileExt);
        int max = 0;

        for(auto file : qAsConst(files)) {
            const auto match = expr.match(file);
            if (match.hasMatch()) {
                const int index = match.captured(2).toInt();

                if (index > max) {
                    max = index;
                    lastLog = file;
                }
            }
        }
    }
    else if (_rotationType == MLog::RotationType::DateTime) {
        const QRegularExpression expr("(" + appName + "-)"
                      "(\\d\\d\\d\\d-\\d\\d-\\d\\d_\\d\\d-\\d\\d-\\d\\d)"
                                + _fileExt);
        QDateTime oldest = QDateTime::currentDateTime();

        for(auto file : qAsConst(files)) {
            const auto match = expr.match(file);
            if (match.hasMatch()) {
                const auto index = QDateTime::fromString(
                            match.captured(2), _dateTimeFormat);

                if (index < oldest) {
                    oldest = index;
                    lastLog = file;
                }
            }
        }
    }
    QFile::remove(logFilePath + '/' + lastLog);
}

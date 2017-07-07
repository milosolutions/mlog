/*******************************************************************************
Copyright (C) 2016 Milo Solutions
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

#include <QtTest>
#include <QCoreApplication>

#include "../mlog.h"

class TestMiloLog : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testEnableLogToFile();
    void testDisableLogToFile();
    void testInThreads();
};

void TestMiloLog::initTestCase()
{
    Q_ASSERT(MLog::instance());
    QCoreApplication::setApplicationName("MiloLog Test");
    QCoreApplication::setOrganizationName("Milo");
}

void TestMiloLog::cleanupTestCase()
{
    QFile::remove(logger()->currentLogPath());
    QFile::remove(logger()->previousLogPath());
}

void TestMiloLog::testEnableLogToFile()
{
    logger()->enableLogToFile(QCoreApplication::applicationName());
    QVERIFY(QFile::exists(logger()->currentLogPath()));
    QVERIFY(!QFile::exists(logger()->previousLogPath()));
    logger()->enableLogToFile("Test log");
    QVERIFY(QFile::exists(logger()->previousLogPath()));
    QVERIFY(logger()->currentLogPath() != logger()->previousLogPath());
    QFile logFile(logger()->currentLogPath());
    quint64 fileSize1 = logFile.size();
    qDebug() << "Test log file size: " << fileSize1;
    quint64 fileSize2 = logFile.size();
    qDebug() << "Test log file size: " << fileSize2;
    QVERIFY(!(fileSize1 == fileSize2));
    quint64 fileSize3 = logFile.size();
    QVERIFY(!(fileSize3 == fileSize2));
    
}

void TestMiloLog::testDisableLogToFile()
{
    logger()->disableLogToFile();
    QVERIFY(QFile::exists(logger()->currentLogPath()));
    QFile logFile(logger()->currentLogPath());
    logFile.size();
    quint64 fileSize1 = logFile.size();
    qDebug() << "Test log file size: " << fileSize1;
    quint64 fileSize2 = logFile.size();logger()->enableLogToFile(QCoreApplication::applicationName());
    qDebug() << "Test log file size: " << fileSize2;
    QCOMPARE(fileSize1, fileSize2);
}

void TestMiloLog::testInThreads()
{
    qDebug() << "TODO: testinThreads";
    //TODO
}

QTEST_MAIN(TestMiloLog)

//#include "tst_milolog.moc"

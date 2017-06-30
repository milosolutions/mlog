#include <QtTest>
#include <QCoreApplication>

#include "milolog.h"

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
    Q_ASSERT(MiloLog::instance());
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

#include "tst_milolog.moc"

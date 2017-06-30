#pragma once

#include <QDebug>

class ExampleClass {
public:
    /*!
     * Please note that inclusion of milolog.h is not needed!
     */
    void logSomething() {
        qDebug().noquote() << "This is a simple test!" << 15;
        qDebug() << "No need to include mlog.h!";
    }
};

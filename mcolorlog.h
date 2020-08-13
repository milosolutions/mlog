#pragma once

#include <QDebug>

#define redLogColor "\033[1;31m"
#define greenLogColor "\033[1;32m"
#define blueLogColor "\033[1;34m"
#define cyanLogColor "\033[1;36m"
#define endLogColor "\033[0m"

class MColorLog : public QDebug
{
public:
    enum Color {
        Red,
        Green,
        Blue,
        Cyan
    };

    MColorLog(const Color color, const QDebug &other)
        : QDebug(other), m_color(color)
    {
        operator<<(colorBegin(m_color));
    }

    ~MColorLog() {
        operator<<(colorEnd());
    }

    const char *colorBegin(const Color color) const {
        switch (color) {
        case Color::Red:
            return redLogColor;
        case Color::Blue:
            return blueLogColor;
        case Color::Cyan:
            return cyanLogColor;
        case Color::Green:
            return greenLogColor;
        }

        return "";
    }

    const char *colorEnd() const {
        return endLogColor;
    }

private:
    Color m_color = Color::Red;
};


#define mCDebug(color, category, ...) MColorLog(color, QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, category().categoryName()).debug(__VA_ARGS__))
#define mCInfo(color, category, ...) MColorLog(color, QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, category().categoryName()).info(__VA_ARGS__))
#define mCWarning(color, category, ...) MColorLog(color, QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, category().categoryName()).warning(__VA_ARGS__))
#define mCCritical(color, category, ...) MColorLog(color, QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, category().categoryName()).critical(__VA_ARGS__))

#define mDebug(color) MColorLog(color, QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).debug())
#define mInfo(color) MColorLog(color, QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).info())
#define mWarning(color) MColorLog(color, QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).warning())
#define mCritical(color) MColorLog(color, QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).critical())

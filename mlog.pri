QT *= core
CONFIG *= c++11

# Warning! QStringBuilder can crash your app! See last point here:
# https://www.kdab.com/uncovering-32-qt-best-practices-compile-time-clazy/
# !!!
DEFINES *= QT_USE_QSTRINGBUILDER QT_MESSAGELOGCONTEXT
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

INCLUDEPATH *= $$PWD

HEADERS *= $$PWD/mlog.h $$PWD/mlogtypes.h
SOURCES *= $$PWD/mlog.cpp $$PWD/mlogtypes.cpp

OTHER_FILES *= $$PWD/README.md $$PWD/AUTHORS.md $$PWD/mlog.doxyfile

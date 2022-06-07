find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Test)

set(CMAKE_AUTOMOC ON)

add_executable(tst_mlog loggingthread.h loggingthread.cpp tst_mlog.cpp)

target_link_libraries(tst_mlog mlog
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Test
)

add_test(tst_mlog tst_mlog)

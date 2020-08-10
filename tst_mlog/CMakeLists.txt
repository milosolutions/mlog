find_package(Qt5 COMPONENTS REQUIRED Core Test)

set(CMAKE_AUTOMOC ON)

add_executable(tst_mlog loggingthread.h loggingthread.cpp tst_mlog.cpp)

target_link_libraries(tst_mlog mlog
  Qt5::Core Qt5::Test
)

add_test(tst_mlog tst_mlog)

find_package(Qt5 COMPONENTS REQUIRED Core Test)

set(CMAKE_AUTOMOC ON)

add_executable(tst_mlog loggingthread.h loggingthread.cpp tst_mlog.cpp
  $<TARGET_OBJECTS:mlog>
)

target_include_directories(tst_mlog PUBLIC
  $<TARGET_PROPERTY:mlog,INCLUDE_DIRECTORIES>
)

target_link_libraries(tst_mlog
  Qt5::Core Qt5::Test
)

add_test(tst_mlog tst_mlog)

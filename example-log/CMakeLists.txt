find_package(Qt5 COMPONENTS REQUIRED Core Test)

set(CMAKE_AUTOMOC ON)

add_executable(example-log exampleclass.h main.cpp)

target_link_libraries(example-log mlog
  Qt5::Core
)

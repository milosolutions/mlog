find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

set(CMAKE_AUTOMOC ON)

add_executable(example-log exampleclass.h main.cpp)

target_link_libraries(example-log mlog
  Qt${QT_VERSION_MAJOR}::Core
)

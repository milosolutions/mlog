if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif ()

cmake_minimum_required(VERSION 3.5)

project(mlog
  VERSION 0.0.1
  DESCRIPTION "Milo Log"
  LANGUAGES CXX
)

find_package(Qt5 COMPONENTS REQUIRED Core)

set(SOURCES mlog.h mlog.cpp mlogtypes.h mlogtypes.cpp)

set(OTHER_FILES README.md AUTHORS.md mlog.doxyfile)

add_library(mlog STATIC ${SOURCES} ${OTHER_FILES})

target_include_directories(mlog
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(mlog
  Qt5::Core
)

add_subdirectory(tst_mlog)
add_subdirectory(example-log)

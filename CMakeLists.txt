
project(mlog
  VERSION 0.0.1
  DESCRIPTION "Milo Log"
  LANGUAGES CXX
)

find_package(Qt5 COMPONENTS REQUIRED Core)

set(SOURCES mlog.h mlog.cpp)

set(OTHER_FILES README.md AUTHORS.md mlog.doxyfile)

add_library(mlog OBJECT ${SOURCES} ${OTHER_FILES})

target_include_directories(mlog
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(mlog
  Qt5::Core
)

add_subdirectory(tst_mlog)

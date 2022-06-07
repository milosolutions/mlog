cmake_minimum_required(VERSION 3.5)

project(mlog
  VERSION 0.0.1
  DESCRIPTION "Milo Log"
  LANGUAGES CXX
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

set(SOURCES mlog.h mlog.cpp mlogtypes.h mlogtypes.cpp mcolorlog.h)

set(OTHER_FILES README.md AUTHORS.md mlog.doxyfile)

add_library(mlog STATIC ${SOURCES} ${OTHER_FILES})

target_include_directories(mlog
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

if (ANDROID)
  # From:
  # https://stackoverflow.com/questions/40844163/android-ndkcmake-undefined-reference-to-android-log-write-when-using-log
  find_library(log-lib log)
  target_link_libraries(mlog
    Qt${QT_VERSION_MAJOR}::Core
    ${log-lib}
  )
else()
  target_link_libraries(mlog
    Qt${QT_VERSION_MAJOR}::Core
  )
endif()

add_subdirectory(tst_mlog)
add_subdirectory(example-log)

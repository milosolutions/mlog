
project(mlog
  VERSION 0.0.1
  DESCRIPTION "Milo Log"
  LANGUAGES CXX
)

include(../../qtbase.cmake)

set(SOURCES mlog.h mlog.cpp)

set(OTHER_FILES README.md AUTHORS.md mlog.doxyfile)

add_library(mlog OBJECT ${SOURCES} ${OTHER_FILES})

target_include_directories(mlog
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# Mark all warnings as errors
target_compile_options(mlog PRIVATE -Werror -fPIC)

# Enable LTCG (link time optimization)
option (ltcg "Enable LTCG (link time optimization)" OFF)
if (ltcg)
  set_property(TARGET mlog PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif (ltcg)

# Use C++ 14 standard
target_compile_features(mlog PRIVATE cxx_std_14)

# Defines
target_compile_definitions(mlog PRIVATE QT_USE_QSTRINGBUILDER)

target_link_libraries(mlog
  Qt5::Core
)

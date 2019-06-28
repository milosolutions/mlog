
include(../../../qtbase.cmake)

find_package(Qt5 COMPONENTS REQUIRED Core Test)

#add_library(tests OBJECT ${SOURCES} ${OTHER_FILES})
add_executable(tst_mlog loggingthread.h loggingthread.cpp tst_mlog.cpp
  $<TARGET_OBJECTS:mlog>
)

target_include_directories(tst_mlog PUBLIC
  $<TARGET_PROPERTY:mlog,INCLUDE_DIRECTORIES>
)

## Mark all warnings as errors
target_compile_options(tst_mlog PRIVATE -Werror -fPIC)

## Enable LTCG (link time optimization)
#option (ltcg "Enable LTCG (link time optimization)" OFF)
#if (ltcg)
#  set_property(TARGET tests PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
#endif (ltcg)

## Use C++ 14 standard
target_compile_features(tst_mlog PRIVATE cxx_std_14)

## Defines
target_compile_definitions(tst_mlog PRIVATE QT_USE_QSTRINGBUILDER)

target_link_libraries(tst_mlog
  Qt5::Core Qt5::Test
)

add_test(tst_mlog tst_mlog)

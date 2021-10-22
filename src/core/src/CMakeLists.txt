set(TARGET core)

file( GLOB_RECURSE LIB_TARGET_HEADERS *.h ../include/${TARGET_NAME}/*.h )
file( GLOB_RECURSE LIB_TARGET_SRCS *.cpp )

add_library(${TARGET} STATIC ${LIB_TARGET_HEADERS} ${LIB_TARGET_SRCS})

# задаём настройки компилятора
if (WIN32)
  # GitHub Actions problem:
  # If you want to have /std:c++latest with CMake 3.21 you need to have set(CMAKE_CXX_STANDARD 23).
  # CMake is setting correctly /std:c++20 for set(CMAKE_CXX_STANDARD 20), but as it turns out some
  # headers (<format>, <ranges) are not included by MSVC with /std:c++20
  # see https://github.com/microsoft/STL/issues/1814#issuecomment-845572895
  set_target_properties(${TARGET} PROPERTIES
    CXX_STANDARD 20  # /std:c++latest
    CXX_STANDARD_REQUIRED ON)
  target_compile_options(${TARGET} PRIVATE /std:c++latest)
else()
  set_target_properties(${TARGET} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)
endif()

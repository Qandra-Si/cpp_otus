set(TARGET "${CMAKE_PROJECT_NAME}_lesson03")

add_executable(${TARGET} main.cpp)

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED ON)

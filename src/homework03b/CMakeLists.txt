set(TARGET "${CMAKE_PROJECT_NAME}_homework03b")

add_executable(${TARGET} main.cpp)

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED ON)

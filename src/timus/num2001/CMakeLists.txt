get_filename_component(TASK_NUM ${CMAKE_CURRENT_LIST_DIR} NAME)
set(TARGET "${CMAKE_PROJECT_NAME}_acm_${TASK_NUM}")

add_executable(${TARGET} main.cpp)

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED ON)

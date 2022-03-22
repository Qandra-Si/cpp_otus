get_filename_component(TARGET_SUBNAME ${CMAKE_CURRENT_LIST_DIR} NAME)
set(TARGET "${CMAKE_PROJECT_NAME}_${TARGET_SUBNAME}")

add_executable(${TARGET} main.cpp)
target_link_libraries(${TARGET} ${CMAKE_THREAD_LIBS_INIT})

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON)
target_compile_options(${TARGET} PRIVATE -O3)

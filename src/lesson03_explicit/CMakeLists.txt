get_filename_component(TARGET_SUBNAME ${CMAKE_CURRENT_LIST_DIR} NAME)
set(TARGET "${CMAKE_PROJECT_NAME}_${TARGET_SUBNAME}")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_executable(${TARGET} main.cpp)

  set_target_properties(${TARGET} PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON)
endif()

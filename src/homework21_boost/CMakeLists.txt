get_filename_component(TARGET_SUBNAME ${CMAKE_CURRENT_LIST_DIR} NAME)
set(TARGET "${CMAKE_PROJECT_NAME}_${TARGET_SUBNAME}")

file( GLOB_RECURSE TARGET_HEADERS *.h)
file( GLOB_RECURSE TARGET_SRCS *.cpp )

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(${TARGET} ${TARGET_HEADERS} ${TARGET_SRCS})
target_link_libraries(${TARGET} ${Boost_LIBRARIES})

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON)

cpp_otus_copy_prerequisites_for_target(${TARGET})

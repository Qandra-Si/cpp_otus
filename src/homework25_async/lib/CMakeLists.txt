set(TARGET async)

file( GLOB_RECURSE LIB_TARGET_HEADERS *.h )
file( GLOB_RECURSE LIB_TARGET_SRCS *.cpp )

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../bulk") # делаем доступным файл interface.h

add_library(${TARGET} STATIC ${LIB_TARGET_HEADERS} ${LIB_TARGET_SRCS})
target_link_libraries(${TARGET} core bulk)

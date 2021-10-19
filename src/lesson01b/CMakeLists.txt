set(TARGET "${CMAKE_PROJECT_NAME}_lesson01b")

add_executable(${TARGET} main.cpp)
target_link_libraries(${TARGET} core)

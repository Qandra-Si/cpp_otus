get_filename_component(TARGET_SUBNAME ${CMAKE_CURRENT_LIST_DIR} NAME)
set(TARGET "${CMAKE_PROJECT_NAME}_${TARGET_SUBNAME}")

# не переусложняем, - первый урок простой с одним файлом main.cpp, - его и подключаем
add_executable(${TARGET} main.cpp)

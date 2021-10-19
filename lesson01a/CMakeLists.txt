set(TARGET "${CMAKE_PROJECT_NAME}_lesson01a")

# генерируем config.h файл по шаблону в .in файле
configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# не переусложняем, - первый урок простой с одним файлом main.cpp, - его и подключаем
add_executable(${TARGET} main.cpp)

# требуем, чтобы компилятор поддерживал c++17, хоть эта опция и не нужна в первом уроке, - пусть будет ради примера
set_target_properties(
  ${TARGET} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

# сообщаем компилятору где искать сгенерированный config.h файл
target_include_directories(
  ${TARGET}
  PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
)

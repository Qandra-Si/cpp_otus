set(TARGET "${CMAKE_PROJECT_NAME}_lesson01a")

# не переусложняем, - первый урок простой с одним файлом main.cpp, - его и подключаем
add_executable(${TARGET} main.cpp)

# требуем, чтобы компилятор поддерживал c++17, хоть эта опция и не нужна в первом уроке, - пусть будет ради примера
set_target_properties(
  ${TARGET} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

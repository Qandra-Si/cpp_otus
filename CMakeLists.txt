# выбираем версию cmake не менее, чем 3.16 потому что она у меня есть ;)
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# выбираем название проекта, пусть это будет акроним c++ otus cources
# а версия проекта пусть будет 0.10.20, - дата начала курсов
project(cpp_otus VERSION 0.10.20)

# генерируем config.h файл по шаблону в .template файле
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp_otus_config.h.template
  ${CMAKE_CURRENT_BINARY_DIR}/include/cpp_otus_config.h
)
# файл, как и все прочие полезности будет располагаться в каталоге include, его и подключаем
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
# также добавляем в пути наш каталог с include подкаталогами
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# добавляем каталог, гду будут искаться остальные cmake-файлы
add_subdirectory(src)


# выбираем версию cmake не менее, чем 3.16 потому что она у меня есть ;)
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# выбираем название проекта, пусть это будет акроним c++ otus cources
# а версия проекта пусть будет 0.10.20, - дата начала курсов
project(cpp_otus VERSION 0.10.20)

# начинаем добавлять директории, в которых лежат наши выполненные задания
add_subdirectory(lesson01)
add_subdirectory(lesson01a)

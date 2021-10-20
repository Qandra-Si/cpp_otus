# -*- mode: cmake; coding: utf-8 -*-
# C++ Developer Professional Otus' Course (cpp_otus)

#----------------------------------------------------------------
# настраиваем проект
# выбираем версию cmake не менее, чем 3.16 потому что надо следить
# за обновлениями этого инструмента
# выбираем название проекта, пусть это будет акроним от c++ otus
#----------------------------------------------------------------
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(cpp_otus)
set(CMAKE_PROJECT_DESCRIPTION "C++ Developer Professional Otus' Course (cpp_otus)")

#----------------------------------------------------------------
# подготовка префикса всех бинарников, которые будут устанавливаться из пакета
# для, например, получения имён типа ${CMAKE_PROJECT_PREFIX}_lesson01 = cpp_otus_lesson01
#----------------------------------------------------------------
string(TOLOWER ${CMAKE_PROJECT_NAME} CMAKE_PROJECT_LOWERCASE_NAME)
set(CMAKE_PROJECT_PREFIX ${CMAKE_PROJECT_LOWERCASE_NAME})

#----------------------------------------------------------------
# настройка версии проекта
# пусть версия будет 0.10.20, - дата начала курсов
#----------------------------------------------------------------
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 10)
set(PROJECT_VERSION_PATCH 21)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}_${PROJECT_VERSION_MINOR})
math(EXPR PROJECT_VERSION_FULL  "${PROJECT_VERSION_MAJOR} * 100000 + ${PROJECT_VERSION_MINOR} * 100")
string(TIMESTAMP CMAKE_PROJECT_BUILD_DATE "%Y%m%d")

#----------------------------------------------------------------
# поиск дополнительных .cmake файлов
#----------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(CppOtusHelpers)
include(CppOtusAddSubdirectory)

#----------------------------------------------------------------
# конфигурируем проект
# сообщаем сборщику, где желаем видеть продукты сборки
#----------------------------------------------------------------
cpp_otus_set_output_paths()

#----------------------------------------------------------------
# настраиваем флаги компилятора отвечающие за стандарт C++
#----------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)

#----------------------------------------------------------------
# получаем названия solution-ов
#----------------------------------------------------------------
if (DEFINED SOLUTION)
  string(TOUPPER ${SOLUTION} SOLUTION_UPPERCASE)
  string(TOLOWER ${SOLUTION} SOLUTION_LOWERCASE)
endif()

#----------------------------------------------------------------
# конфигурирование информации о версии
# генерируем cpp_otus_version.h файл по шаблону в .template файле
#----------------------------------------------------------------
configure_file(
  ${PROJECT_SOURCE_DIR}/include/cpp_otus_version.h.template
  ${CMAKE_CURRENT_BINARY_DIR}/_generated_/cpp_otus_version.h
)

# подключаем систему тестирования
enable_testing()

#----------------------------------------------------------------
# настройка include-директорий
#----------------------------------------------------------------
include_directories("${CMAKE_CURRENT_BINARY_DIR}/_generated_") # делаем доступным файл cpp_otus_version.h
include_directories(${PROJECT_SOURCE_DIR}/include) # заголовочные файлы cpp_otus-проекта

#----------------------------------------------------------------
# добавление проектов
#----------------------------------------------------------------
message(STATUS "Project configured for build type ${CMAKE_BUILD_TYPE}")
add_subdirectory(src)

#----------------------------------------------------------------
# отладка
#----------------------------------------------------------------
if (PRINT_ALL_VARS)
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endif()

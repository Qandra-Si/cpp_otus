# -*- mode: cmake; coding: utf-8 -*-
# C++ Developer Professional Otus' Course (cpp_otus)

#----------------------------------------------------------------
# настраиваем проект
# выбираем название проекта, пусть это будет акроним от c++ otus
#----------------------------------------------------------------
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(cpp_otus)
set(CMAKE_PROJECT_DESCRIPTION "C++ Developer Professional Otus' Course (cpp_otus)")

#----------------------------------------------------------------
# настраиваем свойства проекта
#----------------------------------------------------------------
if ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" VERSION_GREATER 3.12.2)
  if (POLICY CMP0057 )
    cmake_policy(SET CMP0074 NEW) # find_package uses <PackageName>_ROOT variables
  endif(POLICY CMP0057)
endif("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" VERSION_GREATER 3.12.2)

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

#----------------------------------------------------------------
# подключаем систему тестирования
# используется как ctest, так и gtest (последний опционально)
#----------------------------------------------------------------
enable_testing()

#----------------------------------------------------------------
# настройка gtest
# ...можно далее установить GTEST_MSVC_SEARCH, если надо выбрать /MT или /MD runtime-библиотеку
#----------------------------------------------------------------
if (NOT CPP_OTUS_SKIP_TEST)
  find_package(GTest REQUIRED)
endif()
if (GTEST_FOUND)
  if(NOT WIN32)
    # threads нужен gtest-у в linux
    find_package(Threads REQUIRED)
  endif(NOT WIN32)
  set(CPP_OTUS_GTEST_FOUND TRUE)
  if(MSVC11)
    add_definitions(-D_VARIADIC_MAX=10 ) #vs2012 workaround (да, там были проблемы...)
  endif(MSVC11)
  include_directories("${GTEST_SOURCE_DIR}/include")
else()
  set(CPP_OTUS_GTEST_FOUND FALSE)
endif()

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

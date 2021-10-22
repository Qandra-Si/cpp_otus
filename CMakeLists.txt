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
set(PROJECT_VERSION_PATCH 22)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}_${PROJECT_VERSION_MINOR})
math(EXPR PROJECT_VERSION_FULL  "${PROJECT_VERSION_MAJOR} * 100000 + ${PROJECT_VERSION_MINOR} * 100")
string(TIMESTAMP CMAKE_PROJECT_BUILD_DATE "%Y%m%d")

#----------------------------------------------------------------
# поиск дополнительных .cmake файлов
#----------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(CppOtusHelpers)
include(CppOtusTests)
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
# настройка сборки инсталлятора : пути по умолчанию в разных ОС
#----------------------------------------------------------------
if ((CMAKE_SYSTEM_NAME MATCHES "OpenBSD") OR (CMAKE_SYSTEM_NAME MATCHES "FreeBSD"))
  set (CMAKE_INSTALL_PREFIX "/usr/local")
  message (STATUS "System is OpenBSD/FreeBSD/OS X; install path prefix is set to ${CMAKE_INSTALL_PREFIX}.")
elseif(CMAKE_SYSTEM_NAME MATCHES "NetBSD")
  set (CMAKE_INSTALL_PREFIX "/usr/pkg")
  message (STATUS "System is NetBSD; install path prefix is set to ${CMAKE_INSTALL_PREFIX}.")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set (CMAKE_INSTALL_PREFIX "/usr")
  message (STATUS "Defaulting install path prefix to ${CMAKE_INSTALL_PREFIX}.")
endif()
set(CPP_OTUS_INSTALL_BIN_DIR "bin" CACHE PATH "Installation directory for executables")
set(CPP_OTUS_INSTALL_LIB_DIR "lib" CACHE PATH "Installation directory for libraries")
set(CPP_OTUS_INSTALL_INC_DIR "include" CACHE PATH "Installation directory for headers")
#set(CPP_OTUS_INSTALL_MAN_DIR "share/man" CACHE PATH "Installation directory for manual pages")
#set(CPP_OTUS_INSTALL_PKGCONFIG_DIR "lib/pkgconfig" CACHE PATH "Installation directory for pkgconfig (.pc) files")
set(CPP_OTUS_INSTALL_EXA_DIR "examples" CACHE PATH "Installation directory for examples")
if (WIN32)
  set(CPP_OTUS_INSTALL_ETC_DIR "config" CACHE PATH "Installation directory for windows config files")
else(WIN32)
  set(CPP_OTUS_INSTALL_ETC_DIR "conf" CACHE PATH "Installation directory for linux config files")
endif()

#----------------------------------------------------------------
# настройка cpack
#----------------------------------------------------------------
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CMAKE_PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "Qandra Si")
set(CPACK_PACKAGE_CONTACT "qandra.si@gmail.com")
#set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
#set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
#set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")
#set(CPACK_RESOURCE_FILE_WELCOME "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
if (DEFINED SOLUTION)
  set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH}-${SOLUTION_LOWERCASE})
else()
  set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
endif()

#----------------------------------------------------------------
# настройка cpack генераторов
#----------------------------------------------------------------
if (WIN32 AND NOT UNIX)
  if (NOT CPACK_GENERATOR)
    set(CPACK_GENERATOR "NSIS")
  endif()
  if (${CPACK_GENERATOR} STREQUAL "NSIS")
    set(CPACK_NSIS_COMPONENT_INSTALL ON)
  endif()
else()
  #set(CPACK_GENERATOR "RPM")
  set(CPACK_GENERATOR "DEB")
  #set(CMAKE_SKIP_BUILD_RPATH FALSE)
  #set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  #set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib/cpp_otus)
  #set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  if(${CPACK_GENERATOR} STREQUAL "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
    #set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/${CMAKE_PROJECT_NAME}")
  endif()
endif()
include(CPackComponent)
include(InstallRequiredSystemLibraries)
include(CPack)

#----------------------------------------------------------------
# конфигурирование информации о версии
# генерируем cpp_otus_version.h файл по шаблону в .template файле
#----------------------------------------------------------------
if (WIN32)
  configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/package_file_name.bat.template
    ${CMAKE_CURRENT_BINARY_DIR}/_generated_/package_file_name.bat
  )
else()
  configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/package_file_name.sh.template
    ${CMAKE_CURRENT_BINARY_DIR}/_generated_/package_file_name.sh
  )
endif()
configure_file(
  ${PROJECT_SOURCE_DIR}/include/cpp_otus_version.h.template
  ${CMAKE_CURRENT_BINARY_DIR}/_generated_/cpp_otus_version.h
)

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

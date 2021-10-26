// -*- mode: c++; coding: utf-8 -*-
#pragma once

/****************************************************************************************************************
      Определяем платформу
 ****************************************************************************************************************/

/*! \brief Конфигурация платформы
 *
 * Файл <cpp_otus_config.h>
 *
 * Определяются следующие платформы:
 *  \li \c  Linux   - макрос \b CPP_OTUS_LINUX
 *  \li \c  BSD     - макрос \b CPP_OTUS_BSD
 *  \li \c  Solaris - макрос \b CPP_OTUS_SOLARIS
 *  \li \c  QNX     - макрос \b CPP_OTUS_QNX
 *  \li \c  MacOS   - макрос \b CPP_OTUS_APPLE
 *  \li \c  Unix (неизветная версия Unix системы) - макрос \b CPP_OTUS_UNINX
 *  \li \c  Windows - макрос \b CPP_OTUS_WINODWS
 *  \li \c  Cygwin  - макрос \b CPP_OTUS_CYGWIN
 *
 * Правила конфигурации платоформы:
 *  \li \c макрос \b CPP_OTUS_PLATFORM содержит название (строковое представление) платформы
 *  \li \c макрос \b CPP_OTUS_POSIX    определяем является ли данная ОС истиной(правильной) и подерживает POSIX (все платформы кроме Windows)
 *
 *  \note Если платформа определна как Cygwin - то макрос \b CPP_OTUS_WINDOWS НЕ доступен, однако определен макрос \b CPP_OTUS_POSIX
 */

// linux, also other platforms (Hurd etc) that use GLIBC, should these really have their own config headers though?
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
#  define CPP_OTUS_LINUX
#  define CPP_OTUS_POSIX
#  define CPP_OTUS_PLATFORM "Linux"
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__) // BSD:
#  define CPP_OTUS_BSD
#  define CPP_OTUS_POSIX
#  define CPP_OTUS_PLATFORM "BSD"
#elif defined(sun) || defined(__sun) // solaris:
#  define CPP_OTUS_SOLARIS
#  define CPP_OTUS_POSIX
#  define CPP_OTUS_PLATFORM "Solaris"
#elif defined(__CYGWIN__) // cygwin is not win32:
#  define CPP_OTUS_CYGWIN
#  define CPP_OTUS_POSIX
#  define CPP_OTUS_PLATFORM "Cygwin"
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WINDOWS) // win32:
#  define CPP_OTUS_WINDOWS
#  define CPP_OTUS_PLATFORM "Windows"
#  if defined(__MINGW32__)  //  Get the information about the MinGW runtime, i.e. __MINGW32_*VERSION.
#     include <_mingw.h>
#  endif
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__) // MacOS
#  define CPP_OTUS_APPLE
#  define CPP_OTUS_POSIX
#  define CPP_OTUS_PLATFORM "MacOS"
#elif defined(__QNXNTO__)  // QNX:
#  define CPP_OTUS_QNIX
#  define CPP_OTUS_POSIX
#  define CPP_OTUS_PLATFORM "QNX"
#elif defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
#  define CPP_OTUS_UNINX
#  define CPP_OTUS_POSIX
#  define CPP_OTUS_PLATFORM "Unix"
#else
#   error "Unknown platform - please reconfigure"
#endif

/****************************************************************************************************************
      Определяем компилятор
****************************************************************************************************************/

#if defined(_MSC_VER) && defined(__clang__)
#  error Unsupported compilers configuration :(
#endif

/*! \brief Конфигурация компилятора
 *
 * Файл <cpp_otus_config.h>
 *
 * Определяются следующие компиляторы:
 *  \li \c  GCC (включая MinGW) - макросы \b CPP_OTUS_CXX_GCC, \b CPP_OTUS_CXX_GCC_VER
 *  \li \c  LLVM clang          - макросы \b CPP_OTUS_CXX_CLANG, \b CPP_OTUS_CXX_CLANG_VER
 *  \li \c  Bolrand             - макросы \b CPP_OTUS_CXX_BORLAND, \b CPP_OTUS_CXX_BORLAND_VER
 *  \li \c  MS Visual C         - макросы \b CPP_OTUS_CXX_MSCm \b CPP_OTUS_CXX_MSC_VER
 *
 *  Макрос \b CPP_OTUS_CXX  содержит название (строковое представление) компилятора
*/
#if defined(__GNUC__)
#  define CPP_OTUS_CXX_GCC
#  define CPP_OTUS_CXX "gcc"
#  define CPP_OTUS_CXX_GCC_VER (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#  if CPP_OTUS_CXX_GCC_VER < 40000
#    error "Unsuported gcc version"
#  endif
#elif defined(__BORLANDC__) || defined(__BCPLUSPLUS__)
#  define CPP_OTUS_CXX_BORLAND
#  define CPP_OTUS_CXX "bcb"
#  error "Version of CPP_OTUS_CXX_BORLAND_VER not specified!" // доделать
#elif defined(_MSC_VER)
#  define CPP_OTUS_CXX_MSC
#  define CPP_OTUS_CXX "MS VisualC"
#  define CPP_OTUS_CXX_MSC_VER (_MSC_VER)
#else
#  error "Unknown compiler - please reconfigure"
#endif

#if defined(__clang__)
#  define CPP_OTUS_CXX_CLANG
#  if defined(CPP_OTUS_CXX_GCC)
#    undef CPP_OTUS_CXX // случай, когда CMAKE_C_COMPILER=gcc CMAKE_CXX_COMPILER=clang++ (сборка по умолчанию в QtCreator)
#  endif
#  define CPP_OTUS_CXX "clang"
#  if defined(__apple_build_version__) // Apple and LLVM's Clang. Apple Clang version 7.0 roughly equals LLVM Clang version 3.7
#    define CPP_OTUS_CXX_APPLE_CLANG_VER (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#  else
#    define CPP_OTUS_CXX_LLVM_CLANG_VER (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#  endif // __apple_build_version__
#endif

// Compatibility with non-clang compilers.
#ifndef __has_feature
#  define __has_feature(x) 0
#endif

// см. также https://github.com/weidai11/cryptopp/blob/master/config.h

/****************************************************************************************************************
      Макросы экспорта/импорта функций
****************************************************************************************************************/

/*! \brief Макросы экспорта/импорта функци
 *
 * Файл <cpp_otus_config.h>
 *
 * Определены следующие макросы облегчающие жизнь при переносе кода между платофрмами:
 *   \li \c \b CPP_OTUS_SYMBOL_EXPORT - макрос определяет экспортируемые ф-ции для Windows библиотек
 *   \li \c \b CPP_OTUS_SYMBOL_IMPORT - макрос определяет импортируемые ф-ции для Windows библиотек
 *   \li \c \b CPP_OTUS_STDCALL       - соглашение о вызове __stdcall
 *   \li \c \b CPP_OTUS_CDECL         - соглашение о вызове __cdecl
*/
#if defined(__GNUC__) && (__GNUC__ >= 4) && (defined(_WIN32) || defined(__WIN32__) || defined(WIN32)) && !defined(__CYGWIN__)
   // All Win32 development environments, including 64-bit Windows and MinGW, define
   // _WIN32 or one of its variant spellings. Note that Cygwin is a POSIX environment,
   // so does not define _WIN32 or its variants.
#  define CPP_OTUS_SYMBOL_EXPORT __attribute__((dllexport))
#  define CPP_OTUS_SYMBOL_IMPORT __attribute__((dllimport))
#endif
#if defined(CPP_OTUS_WINDOWS)
#  ifndef CPP_OTUS_SYMBOL_EXPORT
#    define CPP_OTUS_SYMBOL_EXPORT __declspec(dllexport)
#  endif
#  ifndef CPP_OTUS_SYMBOL_IMPORT
#    define CPP_OTUS_SYMBOL_IMPORT __declspec(dllimport)
#  endif
#  define CPP_OTUS_STDCALL __stdcall
#  define CPP_OTUS_CDECL   __cdecl
#else
#  define CPP_OTUS_SYMBOL_EXPORT
#  define CPP_OTUS_SYMBOL_IMPORT
#  define CPP_OTUS_STDCALL
#  define CPP_OTUS_CDECL
#endif

/****************************************************************************************************************
      если Windows, используем MINIMAL INCLUDEs
****************************************************************************************************************/

#if defined(MSSC_WINDOWS)
#  if !defined(WIN32_LEAN_AND_MEAN)
#    define WIN32_LEAN_AND_MEAN
#  endif
#endif


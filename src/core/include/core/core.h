// -*- mode: c++; coding: utf-8 -*-
/*! \file matrix.h
* \brief Конфигурационный файл для библиотеки libcore
*/

#pragma once

#include <cpp_otus_config.h>

/*! \defgroup core Библиотека libcore, в составе проекта C++ Developer Professional Otus' Course (cpp_otus)
*
* Библиотека libcore многократно использовалась в ходе домашних работ курса
* (подробнее см. информацию в корневом README.md файле). В группе core собраны
* методы, шаблоны и классы, используемые в приложениях проекта. Реализации
* библиотеки протестированы с использованием google_test, также для них дана
* подробная документация в этой секции.
*/

/*! \addtogroup core
* @{
*/

#if defined(CPP_OTUS_ALL_DYN_LINK) || defined(CPP_OTUS_CORE_DYN_LINK)
#  if defined( CPP_OTUS_CORE_SOURCE )
#    define CPP_OTUS_CORE_DECL CPP_OTUS_SYMBOL_EXPORT
#  else
#    define CPP_OTUS_CORE_DECL CPP_OTUS_SYMBOL_IMPORT
#    define CPP_OTUS_DYN_LINK
#  endif
#endif

#ifndef CPP_OTUS_CORE_DECL
#  define CPP_OTUS_CORE_DECL
#endif

/*! @} */

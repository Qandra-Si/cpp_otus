// -*- mode: c++; coding: utf-8 -*-
/*! \file initializer.h
* \brief Настройки запуска домашнего задания 21ой лекции
*/

#pragma once

#include <vector>
#include <string>

#include <boost/optional.hpp>


/*! \defgroup homework21 Интерфейсные классы программы поиска файлов на диске
*
* Подробное описание назначения классов см. в корневом README.md файле.
*/

/*! \addtogroup homework21
* @{
*/

namespace homework21 {

/*! \brief Поддерживаемые алгоритмы хеширования
*/
enum class algorithm_t
{
  crc32,
  md5,
};

/*! \brief Настройки программы
*/
struct startup_params_t
{
  std::vector<std::string> path; /*!< директории для сканирования */
  std::vector<std::string> exclude_path; /*!< директории для исключения из сканирования */
  boost::optional<unsigned> level; /*!< уровень сканирования (один на все директории, 0 - только указанная директория без вложенных) */
  unsigned long long min_file_size; /*!< минимальный размер файла, октет */
  std::vector<std::string> filename_masks; /*!< маски имен файлов разрешенных для сравнения (не зависят от регистра) */
  boost::optional<unsigned> block_size{ 8192 }; /*!< размер блока, которым производятся чтения файлов */
  algorithm_t algorithm{ algorithm_t::crc32 }; /*!< алгоритм хэширования (crc32, md5) */

  bool verbose; /*!< работа в отладочном (verbose) режиме */
};

bool init_startup_params(int ac, char** av, startup_params_t& params);

}

/*! @} */

// -*- mode: c++; coding: utf-8 -*-
/*! \file utils.h
 * \brief Методы-утилиты, экспортированные из библиотеки libcore
*/

#pragma once

#include <string>
#include <vector>

#include <core/core.h>


/*! \addtogroup core
 * @{
*/
namespace core {

/*! \brief Возвращает версию проекта, заданную в корневом CMakeLists.txt
* \ result Строка, в форме "0.10.20"
*/
std::string get_build_ticker();

/*! \brief Проверяет тип платформы little/big-endian
*/
bool is_little_endian();

/*! \brief Разделяет строку на лексемы используя символ разделитель
* \param s Строка с лексемами
* \param delimiter Символ-разделитель
* \return Результат разделения строки, контейнер с лексемами
*/
std::vector<std::string> split(const std::string& s, char delimiter);

/*! \brief Разделяет строку на лексемы используя символ разделитель
* \param s Строка с лексемами
* \param delimiter Символ-разделитель
* \param Результат разделения строки, контейнер с лексемами
*/
void split_into(const std::string& s, char delimiter, std::vector<std::string> & into);

}

/*! @} */

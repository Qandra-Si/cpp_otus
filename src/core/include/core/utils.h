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

/*! \brief Удаляет пробельные символы из начала и конца строки
* \param s Строка, которую требуется "очистить"
* \param whitespaces Символы-разделители, которые будут удалены в начале и конце
* \return Очищенная строка, без whitespaces символов
*
* Рекомендуется использовать впаре с кодом чтения строк из cin, например с
* помощью getline. Но getline, к сожалению, дочитывает до конца строки после
* полученной лексемы, т.ч. если на следующей позиции будут найдены whitespaces,
* то они попадут на вход как пустая или заполненная пробелами строка, - удаляем
* их.
*/
std::string trim(const std::string& s, const char * whitespaces = " \n\r\t\f\v");

}

/*! @} */

// -*- mode: c++; coding: utf-8 -*-
#pragma once

#include <string>
#include <vector>


namespace core {

/*! \brief Возвращает версию проекта, заданную в корневом CMakeLists.txt
* \ result Строка, в форме "0.10.20"
*/
std::string get_build_ticker();

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

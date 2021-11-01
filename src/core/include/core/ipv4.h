// -*- mode: c++; coding: utf-8 -*-
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>

#include <cpp_otus_config.h>


namespace core {

/*! \brief Определение IPv4 адреса
*/
using ipv4_t = std::array<uint8_t, 4>;

/*! \brief Определение пула IPv4 адресов
*/
using ip_pool_t = std::vector<ipv4_t>;

/*! \brief Разбор IPv4 адреса из строкового в \ref ipv4_t представление
* \param s Строковое представление адреса, например 192.168.0.1
* \param ipv4 Результат разбора строки в числовую \ref ipv4_t форму
* \result Признак успешного завершения разбора адреса на числа
*/
bool split_ipv4(const std::string& s, ipv4_t& ipv4);

typedef int(CPP_OTUS_CDECL* TYPEOF_ipv4_cmp)(const ipv4_t*, const ipv4_t*);

/*! \brief Сортировка пула IP4v адресов заданным методом сравнения
* \param pool Пул IPv4 адресов
* \pred pred Метод сравнения, см. описание метода сравнения в std::qsort
*/
void sort_ipv4_pool(ip_pool_t& pool, TYPEOF_ipv4_cmp pred);

/*! \brief Сравнение пары IPv4 адресов для big-endian пларформ
*/
int big_endian_ipv4_cmp(const ipv4_t* x, const ipv4_t* y);

/*! \brief Сравнение пары IPv4 адресов для little-endian пларформ
*/
int little_endian_ipv4_cmp(const ipv4_t* x, const ipv4_t* y);

/*! \brief Сравнение пары IPv4 адресов для big-endian пларформ (обратное направление)
*/
inline int big_endian_ipv4_cmp_reverse(const ipv4_t* x, const ipv4_t* y) { return -big_endian_ipv4_cmp(x, y); }

/*! \brief Сравнение пары IPv4 адресов для little-endian пларформ (обратное направление)
*/
inline int little_endian_ipv4_cmp_reverse(const ipv4_t* x, const ipv4_t* y) { return -little_endian_ipv4_cmp(x, y); }

}

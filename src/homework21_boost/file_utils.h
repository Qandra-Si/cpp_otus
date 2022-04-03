// -*- mode: c++; coding: utf-8 -*-
/*! \file file_utils.h
* \brief Утилиты для работы с файлами домашнего задания 21ой лекции
*/

#pragma once

#include <array>

#include "initializer.h"


/*! \addtogroup homework21
* @{
*/

namespace homework21 {

/*! \brief Размер файла
*/
using file_size_t = unsigned long long;

/*! \brief Расчёт crc32 хеш-суммы по первому блоку файла
* \param fname Полный путь к файлу
* \param fsize Размер файла
* \param fblock Размер блока файла по которому будет считаться хеш
*/
uint32_t get_hash_crc32(const std::string& fname, file_size_t fsize, unsigned fblock);

/*! \brief Расчёт md5 хеш-суммы по первому блоку файла
* \param fname Полный путь к файлу
* \param fsize Размер файла
* \param fblock Размер блока файла по которому будет считаться хеш
*/
std::array<uint32_t, 4> get_hash_md5(const std::string& fname, file_size_t fsize, unsigned fblock);

/*! \brief Список файлов для сравнения
*/
using filenames_t = std::vector<std::string>;

/*! \brief Сравнение файлов одинаковой длины между собой используя crc32 хеш
* \param fnames Полные пути к файлам
* \param fsize Размеры файлов
* \param fblock Размер блока файла по которому будет считаться хеш
*/
std::vector<filenames_t> compare_files_crc32(const filenames_t& fnames, file_size_t fsize, unsigned fblock);

/*! \brief Сравнение файлов одинаковой длины между собой используя md5 хеш
* \param fnames Полные пути к файлам
* \param fsize Размеры файлов
* \param fblock Размер блока файла по которому будет считаться хеш
*/
std::vector<filenames_t> compare_files_md5(const filenames_t& fnames, file_size_t fsize, unsigned fblock);

}

/*! @} */

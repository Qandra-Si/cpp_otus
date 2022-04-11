// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <unordered_map>

#include <core/utils.h>

#include "initializer.h"
#include "file_utils.h"

#include <boost/filesystem.hpp>
#include <boost/functional/hash.hpp>



namespace fs = boost::filesystem; // начиная с C++17 можно пользоваться std::filesystem

using files_by_sz_t = std::multimap<homework21::file_size_t, std::string>;
std::size_t decimation_unique_sizes(files_by_sz_t& files_by_sz);

void print_identical_files(const files_by_sz_t& files_by_sz, const homework21::startup_params_t& params);

int main(int argc, char* argv[])
{
  // инициализация работы программы с помощью командной строки
  homework21::startup_params_t params;
  if (!homework21::init_startup_params(argc, argv, params)) return EXIT_FAILURE;

  // подготовка локальных переменных для работы цикла
  files_by_sz_t files_by_sz;

  for (const auto& path : params.path)
  {
    // рекурсивный перебор файлов
    const fs::recursive_directory_iterator end;
    fs::recursive_directory_iterator itr = fs::recursive_directory_iterator(path);
    while (itr != end)
    {
      const fs::directory_entry& e = *itr;
      if (e.status().type() == fs::file_type::regular_file)
      {
        // проверка ограничения на размер файла, если слишком маленький, то пропускаем
        homework21::file_size_t sz = fs::file_size(e.path());
        if (sz < params.min_file_size)
        {
          if (params.verbose) std::cout << "skip file " << e.path() << " (by size " << sz << ")" << std::endl;
          ++itr;
          continue;
        }
        // проверка шаблонов файлов (если указаны)
        if (!params.filename_masks.empty())
        {
          const std::string fname = e.path().filename().string();
          if (params.filename_masks.end() == find_if(params.filename_masks.begin(), params.filename_masks.end(),
            [&fname](const std::regex& tmpl) {
              return std::regex_match(fname, tmpl);
            }
          ))
          {
            if (params.verbose) std::cout << "skip file " << e.path() << " (filename not matches templates)" << std::endl;
            ++itr;
            continue;
          }
        }
        // проверка, можно ли работать с найденным файлом?
        if ((e.status().permissions() & (fs::perms::owner_read | fs::perms::group_read | fs::perms::others_read)) == 0) continue;
        // просто вывод отладки (если включена)
        if (params.verbose) std::cout << "found regular file " << e.path() << " (size " << sz << ")" << std::endl;
        // считаем хеш-сумму файла
        files_by_sz.insert(files_by_sz_t::value_type(sz, e.path().string()));
      }
      else if (e.status().type() == fs::file_type::directory_file)
      {
        // проверка вложенности директорий (поскольку мы итерируемся рекурсивно, без использования рекурсии, то
        // нельзя настроит итератор заранее, - приходится приходить через одну лишнюю вложенность)
        if (params.level.is_initialized() && (itr.depth() > (int)params.level.value()))
        {
          // просто вывод отладки (если включена)
          if (params.verbose) std::cout << "skip directory " << e.path() << " (by depth)" << std::endl;
          // найдено исключение: итерируемся без рекурсии
          itr.no_push();
          ++itr;
          if (itr == end) break;
          itr.no_push(false);
          continue;
        }
        // проверка директорий, которые надо исключить из поиска
        if (!params.exclude_path.empty())
        {
          if (params.exclude_path.end() != find_if(params.exclude_path.begin(), params.exclude_path.end(),
                                                   [&e](const std::string& exclude) { return e.path().compare(exclude) == 0; } ))
          {
            // просто вывод отладки (если включена)
            if (params.verbose) std::cout << "skip directory " << e.path() << " (by exclude)" << std::endl;
            // найдено исключение: итерируемся без рекурсии
            itr.no_push();
            ++itr;
            if (itr == end) break;
            itr.no_push(false);
            continue;
          }
        }
        // просто вывод отладки (если включена)
        if (params.verbose) std::cout << "directory " << e.path() << " (depth " << itr.depth() << ")" << std::endl;
      }
      ++itr;
    }
  }

  // удаляем из списка файлов (проиндексированных по размеру) те файлы, которые не имеют дубликатов по размеру
  std::size_t cnt_before = files_by_sz.size();
  std::size_t cnt_decimated = decimation_unique_sizes(files_by_sz);

  // просто вывод отладки (если включена)
  if (params.verbose) std::cout << "found " << cnt_before << " files (" << cnt_before-cnt_decimated << " of them with non unique sizes)" << std::endl;

  // вывод на экран названия файлов с идентичным содержимым
  print_identical_files(files_by_sz, params);

  return EXIT_SUCCESS;
}

template<class Multimap>
std::size_t decimate_unique_keys(Multimap& mmap, typename Multimap::iterator begin)
{
  std::size_t decimated = 0;
  files_by_sz_t::const_iterator end = mmap.end(), itr = begin;
  while (itr != end)
  {
    const auto key = itr->first;
    std::size_t cnt = mmap.count(key);
    if (cnt != 1)
      itr = mmap.upper_bound(key);
    else
    {
      itr = mmap.erase(itr);
      end = mmap.end();
      decimated++;
    }
  }
  return decimated;
}

std::size_t decimation_unique_sizes(files_by_sz_t& files_by_sz)
{
  files_by_sz_t::iterator zero_size = files_by_sz.find(0);
  if (zero_size == files_by_sz.end())
    zero_size = files_by_sz.upper_bound(0);
  return decimate_unique_keys(files_by_sz, zero_size);
}

class file_hash_t
{
  std::array<uint32_t, 4> x;
  int y;
public:
  file_hash_t() : x{0, 0, 0, 0}, y(1) {}
  file_hash_t(uint32_t crc32) : x{crc32, 0, 0, 0}, y(1) {}
  file_hash_t(const std::array<uint32_t, 4>& md5) : x(md5), y(4) {}
  std::size_t id() const { return hash_value(*this); }

  bool operator==(file_hash_t const& other) const
  {
    return (y == 1) ? (x[0] == other.x[0]) : (x == other.x);
  }

  friend std::size_t hash_value(file_hash_t const& p)
  {
    if (p.y == 1) return p.x[0];
    std::size_t seed = 0;
    boost::hash_combine(seed, p.x[0]);
    boost::hash_combine(seed, p.x[1]);
    boost::hash_combine(seed, p.x[2]);
    boost::hash_combine(seed, p.x[3]);
    return seed;
  }

  std::size_t operator()(file_hash_t const& s) const noexcept
  {
    return hash_value(s);
  }
};

void print_identical_files(const files_by_sz_t& files_by_sz, const homework21::startup_params_t& params)
{
  files_by_sz_t::const_iterator end = files_by_sz.end();
  files_by_sz_t::const_iterator itr = files_by_sz.find(0);
  if (itr == files_by_sz.end())
    itr = files_by_sz.upper_bound(0);
  while (itr != end)
  {
    homework21::file_size_t sz = itr->first;
    files_by_sz_t::const_iterator next = files_by_sz.upper_bound(sz);
    if (sz == 0)
    {
      // файлы нулевой длины являются одинаковыми ('содержимое' совпадает ;)
      for (; itr != next; ++itr)
        std::cout << itr->second << std::endl;
      std::cout << std::endl;
    }
    else
    {
      // создаём cnt корзин в unordered_multimap-е
      // к сожалению аллокатор boost::unordered_multimap даже последней версии deprecated in C++17
      std::size_t cnt = files_by_sz.count(sz);
      using hashed_files_t = std::unordered_multimap<file_hash_t, std::string, boost::hash<file_hash_t>>;
      hashed_files_t hashed_files;
      hashed_files.reserve(cnt);
      // перебираем все файлы размера sz, расчёт у них хеш-сумм первых блоков
      for (; itr != next; ++itr)
      {
        file_hash_t hash;
        switch (params.algorithm)
        {
        case homework21::algorithm_t::crc32:
          hash = homework21::get_hash_crc32(itr->second, sz, params.block_size);
          break;
        case homework21::algorithm_t::md5:
          hash = file_hash_t(homework21::get_hash_md5(itr->second, sz, params.block_size));
          break;
        default:
          assert(0 && "unsupported hash algorithm");
        }
        hashed_files.insert(hashed_files_t::value_type(hash, itr->second));
        // просто вывод отладки (если включена)
        if (params.verbose) std::cout << "hash " << std::hex << std::setfill('0') << std::setw(8) << hash.id() << " for " << itr->second << std::endl;
      }
      // перебор одинаковых хешей и остаточная проверка содержимого файлов
      hashed_files_t::const_iterator end = hashed_files.end(), itr = hashed_files.begin();
      while (itr != end)
      {
        const auto key = itr->first;
        // прореживание уникальных хешей
        std::size_t cnt = hashed_files.count(key);
        if (cnt == 1)
        {
          itr = hashed_files.erase(itr);
          end = hashed_files.end();
          continue;
        }
        // получаем файлы с одинаковым хешом (первого блока)
        auto range = hashed_files.equal_range(key);
        homework21::filenames_t range_files(cnt);
        auto filename_selector = [](auto pair) { return pair.second; };
        std::transform(range.first, range.second, range_files.begin(), filename_selector);
        std::vector<homework21::filenames_t> compare_result;
        switch (params.algorithm)
        {
        case homework21::algorithm_t::crc32:
          compare_result = homework21::compare_files_crc32(range_files, sz, params.block_size, params.verbose);
          break;
        case homework21::algorithm_t::md5:
          compare_result = homework21::compare_files_md5(range_files, sz, params.block_size, params.verbose);
          break;
        default:
          assert(0 && "unsupported hash algorithm");
        }
        // если идентичные файлы найдены, то вывод их наименований в консоль
        for (const auto& group : compare_result)
        {
          for (const auto& fname : group)
            std::cout << fname << std::endl;
          std::cout << std::endl;
        }
        // удаление из коллекции более ненужных (обработанных) файлов
        do
        {
          itr = hashed_files.erase(itr);
          end = hashed_files.end();
          auto tmp = hashed_files.find(key);
          if (tmp != end) itr = tmp; else break;
        } while (true);
      }
    }
  }
}

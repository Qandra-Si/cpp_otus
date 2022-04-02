// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <cstdlib>

#include "initializer.h"

#include <boost/regex.hpp>
#include <boost/filesystem.hpp>



namespace fs = boost::filesystem;

int main(int argc, char* argv[])
{
  // инициализация работы программы с помощью командной строки
  homework21::startup_params_t params;
  if (!homework21::init_startup_params(argc, argv, params)) return EXIT_FAILURE;

  boost::regex reg("(.*\\.txt)");
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
        unsigned long long sz = fs::file_size(e.path());
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

  return EXIT_SUCCESS;
}

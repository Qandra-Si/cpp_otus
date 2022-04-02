#include <iostream>

#include <cpp_otus_config.h>
#include <cpp_otus_version.h>
#include "initializer.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;


namespace homework21 {

std::istream& operator>> (std::istream& in, algorithm_t& hash)
{
  std::string token;
  in >> token;
  boost::to_lower(token);

  if (token == "crc32")
  {
    hash = algorithm_t::crc32;
  }
  else if (token == "md5")
  {
    hash = algorithm_t::md5;
  }
  else
  {
    throw po::validation_error(po::validation_error::kind_t::invalid_option_value, "invalid hash algorithm");
  }

  return in;
}

bool init_startup_params(int ac, char **av, startup_params_t &params)
{
  po::options_description generic_options("Опции общего назначения"); // параметры, разрешённые для ввода в командной строке
  generic_options.add_options()
    ("version,v", "выводит строку с версией")
    ("help,h", "отображает эту справочную информацию")
    ("path,p", po::value<std::vector<std::string>>()->multitoken()->required(), "директории для сканирования")
    ("exclude,e", po::value<std::vector<std::string>>()->multitoken(), "директории для исключения из сканирования")
    ("level,l", po::value<unsigned>(), "уровень сканирования, 0 - только указанные директории")
    ("min_size,s", po::value<unsigned long long>()->default_value(1ull), "минимальный размер файла, кол-во октет")
    ("template,t", po::value<std::vector<std::string>>()->multitoken(), "маски имен файлов разрешенных для сравнения (регистрозависимы, пример [a-zA-Z0-9]+\\.txt)")
    ("block,b", po::value<unsigned>(), "размер блока, которым производятся чтения файлов")
    ("algorithm,a", po::value<algorithm_t>(), "алгоритм хэширования:\n 0) crc32\n 1) md5")
    ;
  po::options_description hidden_options("Скрытые опции"); // скрытые от пользователя параметры, но разрешённые для ввода в командной строке, и из конфигурационного файла
  hidden_options.add_options()
    ("verbose", "включение режима отладки")
    ;

  po::options_description cmdline_options; // параметры командной строки
  po::options_description visible_options; // видимые пользователю параметры
  cmdline_options.add(generic_options).add(hidden_options);
  visible_options.add(generic_options);

  bool parse_error_detected = false;
  po::variables_map vm;
  try
  {
    po::store(po::parse_command_line(ac, av, cmdline_options/*, po::command_line_style::unix_style, check_multistream_nonconventional_names*/), vm);
    po::notify(vm); // проверяем диапазоны входных значений
  }
  catch (std::exception& e)
  {
    std::cerr << "ОШИБКА! " << e.what() << std::endl << "См. справочную информацию с ключём help." << std::endl;
    parse_error_detected = true;
  }

  if (parse_error_detected || vm.count("help"))
  {
    std::cout << "bayan " PROJECT_VERSION << std::endl << std::endl;
    visible_options.print(std::cout);
    return false;
  }
  if (vm.count("version"))
  {
    std::cout << "bayan " PROJECT_VERSION << std::endl;
    return false;
  }

  // инициализация параметров приложения
  params.path = vm["path"].as<std::vector<std::string>>();
  if (vm.count("exclude"))
  {
    const auto& v = vm["exclude"].as<std::vector<std::string>>();
    params.exclude_path.reserve(v.size());
    for (const auto& e : v)
      params.exclude_path.push_back(boost::filesystem::path(e).remove_trailing_separator().string());
  }
  if (!vm["level"].empty())
    params.level = vm["level"].as<unsigned>();
  params.min_file_size = vm["min_size"].as<unsigned long long>();
  if (vm.count("template"))
  {
    const std::vector<std::string>& t = vm["template"].as<std::vector<std::string>>();
    for (const auto & tmpl : t)
      params.filename_masks.push_back(std::regex(tmpl));
  }
  if (vm.count("block"))
    params.block_size = vm["block"].as<unsigned>();
  if (vm.count("algorithm"))
    params.algorithm = vm["algorithm"].as<algorithm_t>();
  params.verbose = vm.count("verbose") > 0;

  return true;
}

}

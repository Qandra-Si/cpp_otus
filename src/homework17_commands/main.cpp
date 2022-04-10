// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include <core/utils.h>

#include "interface.h"



struct my_bulk_t : public homework15::custom_bulk_t, protected homework15::commands_finalizer_t
{
  my_bulk_t(unsigned n) : homework15::custom_bulk_t(n, this) { }
  virtual void finish(const std::time_t& t, const std::string& line) override;
};

int get_n(int argc, char* argv[]);

int main(int argc, char* argv[])
{
  // Параметр N передается как единственный параметр командной строки в виде целого числа.
  int n = get_n(argc, argv);
  if (n <= 0)
  {
    std::cerr
      << "Usage: bulk <N>" << std::endl
      << "       where N - the number of commands processed at a bulk" << std::endl;
    return EXIT_FAILURE;
  }

  my_bulk_t bulk(n);

  // Команды считываются построчно из стандартного ввода и обрабатываются блоками по N команд.
  // Одна команда - одна строка, конкретное значение роли не играет.
  // Если данные закончились - блок завершается принудительно.
  std::string cmd;
  while (std::getline(std::cin, cmd))
  {
    cmd = core::trim(cmd);
    // Размер блока можно изменить динамически, если перед началом блока и сразу после дать команды `{` и `}`
    // соответственно. Предыдущий пакет при этом принудительно завершается. Такие блоки могут быть включены
    // друг в друга при этом вложенные команды `{` и `}` игнорируются (но не сами блоки). Если данные
    // закончились внутри динамического блока, весь динамический блок игнорируется.
    if (cmd.empty())
      break;
    else if (cmd == "{")
      bulk.start_transaction();
    else if (cmd == "}")
      bulk.commit_transaction();
    else
      bulk.prepare(new homework15::custom_command_t(cmd));
  }

  return EXIT_SUCCESS;
}


int get_n(int argc, char* argv[])
{
  if (argc != 2) return 0;
  try
  {
    int n = std::stoul(argv[1]);
    return n;
  }
  catch (...)
  {
    return 0;
  }
}

void my_bulk_t::finish(const std::time_t& t, const std::string& line)
{
  std::cout << line << std::endl;

  std::string filename = "bulk" + std::to_string(t) + ".log";
  std::ofstream f(filename);
  f << line << std::endl;
  f.close();
}

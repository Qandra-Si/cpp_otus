// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <cstdlib>

#include <core/utils.h>

#include "interface.h"



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

  homework15::custom_bulk_t bulk(n);

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

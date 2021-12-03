// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <cstdlib>

#include <core/utils.h>

class abstract_command_t
{
public:
  virtual std::string execute() = 0;
};

class abstract_bulk_t
{
public:
  virtual void prepare(abstract_command_t* cmd) = 0;
  virtual unsigned start_transaction() = 0;
  virtual unsigned commit_transaction() = 0;
};

class custom_command_t : public abstract_command_t, private std::string
{
public:
  custom_command_t(const std::string& cmd);
  virtual ~custom_command_t() = default;
public: // реализация интерфейса abstract_command_t
  virtual std::string execute() override;
private: // отключаем конструктор и оператор копирования
  custom_command_t(const custom_command_t&) = delete;
  const custom_command_t& operator=(const custom_command_t&) = delete;
};

#include <list>

/*! \brief Набор задержанных к выполнению команд
*/
class commands_processor_t : private std::list<std::unique_ptr<custom_command_t>>
{
  std::time_t t{0};
public:
  commands_processor_t() { }
  void add(abstract_command_t* const cmd);
  void run();
  size_type bulk_size() const { return size(); }
private: // отключаем конструктор и оператор копирования
  commands_processor_t(const commands_processor_t&) = delete;
  const commands_processor_t& operator=(const commands_processor_t&) = delete;
};

class custom_bulk_t : public abstract_bulk_t
{
public:
  custom_bulk_t(unsigned n);
  virtual ~custom_bulk_t();
public: // реализация интерфейса abstract_bulk_t
  virtual void prepare(abstract_command_t* const cmd) override;
  virtual unsigned start_transaction() override;
  virtual unsigned commit_transaction() override;
private:
  const unsigned bulk_size; /*!< кол-во команд в одном bulk, которые выполняются совместно */
  unsigned num_nested_transactions; /*!< кол-во вложенных транзакций */
  commands_processor_t delayed_commands; /*!< список уникальных указателей на отложенные команды */
private: // отключаем конструктор и оператор копирования
  custom_bulk_t(const custom_bulk_t&) = delete;
  const custom_bulk_t& operator=(const custom_bulk_t&) = delete;
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

  custom_bulk_t bulk(n);

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
      bulk.prepare(new custom_command_t(cmd));
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





//-----------------------
#include <cassert>


custom_bulk_t::custom_bulk_t(unsigned n) : bulk_size(n), num_nested_transactions(0)
{
}

custom_bulk_t::~custom_bulk_t()
{
  if (num_nested_transactions == 0)
  {
    delayed_commands.run();
  }
}

void custom_bulk_t::prepare(abstract_command_t* const cmd)
{
  delayed_commands.add(cmd);

  if (num_nested_transactions) return;
  if (delayed_commands.bulk_size() < bulk_size) return;
  delayed_commands.run();
}

unsigned custom_bulk_t::start_transaction()
{
  if (!num_nested_transactions)
  {
    delayed_commands.run();
  }
  return ++num_nested_transactions;
}

unsigned custom_bulk_t::commit_transaction()
{
  assert(("There are no any started transaction", num_nested_transactions > 0));
  if (--num_nested_transactions == 0)
  {
    delayed_commands.run();
  }
  return num_nested_transactions;
}



custom_command_t::custom_command_t(const std::string& cmd) : std::string(cmd)
{
}

std::string custom_command_t::execute()
{
  return *this;
}



#include<ctime>
#include <fstream>

void commands_processor_t::add(abstract_command_t* const _cmd)
{
  custom_command_t* cmd = dynamic_cast<custom_command_t*>(_cmd);
  assert(("There are no any other command's implementation other than this", cmd));

  if (empty()) t = std::time(0);
  emplace_back(value_type(cmd));
}

void commands_processor_t::run()
{
  if (empty()) return;

  std::string line;
  do
  {
    if (line.empty())
      line = "bulk: ";
    else
      line += ", ";
    line += front().get()->execute();
    erase(begin());
  } while (!empty());

  std::cout << line << std::endl;

  std::string filename = "bulk" + std::to_string(t) + ".log";
  std::ofstream f(filename);
  f << line << std::endl;
  f.close();
}

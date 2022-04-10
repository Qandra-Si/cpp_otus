// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <cstdlib>
#include <list>

#include <core/utils.h>

#include "async.h"
#include "logger.h"
#include "threadsafe_writer.h"


volatile static bool g_terminated = false;

int get_n(int argc, char* argv[]);
void init_terminate_handler(homework25::abstract_logger_t* logger);

int main(int argc, char* argv[])
{
  // Параметр N передается как единственный параметр командной строки в виде целого числа.
  int n = get_n(argc, argv);
  if (n <= 0)
  {
    std::cerr
      << "Usage: async <N>" << std::endl
      << "       where N - the number of commands processed at a bulk" << std::endl;
    return EXIT_FAILURE;
  }

  // создаём поток logger-а и запускаем его
  // поток будет все возникающие события асинхронно доставлять в вывод std::cout
  // Внимание! никакой объект не должен быть создан прежде logger-а, также logger разрушается последним
  homework25::logger_t cout_logger(std::cout);
  // получаем интерфес логгера, который передадим другим объектам
  homework25::abstract_logger_t* logger = cout_logger.get_interface();

  // инициализируем обработчик сигнала, который выставит флаг прекращения работы программы
  init_terminate_handler(logger);

  {
    // <thread pool> устроен таким образом, что только <leader> поток может пользоваться
    // набором данных в <handle set> (через интерфейс <reactor>)
    homework25::my_set_of_commands_t commands(g_terminated);
    leaders_followers::thread_pool_t thread_pool(&commands);

    // <thread spawner> запускает поток, который делает join к <thread pool> и ждёт поступления
    // данных в <handle set>
    homework25::thread_manager_t thread_spawner(&thread_pool, g_terminated);
    auto writer1 = thread_spawner.spawn();
    auto writer2 = thread_spawner.spawn();

    // создаём контекст, в рамках которого будут выполняться операции добавления команд
    auto generator1 = homework25::connect(n);

    // здесь реализован вечный цикл, в котором крутится проверка нажатия на Ctrl+C, которые
    // должен нажать пользователь, так что главный поток ничем не занят, кроме проверки
    // значения флага g_terminated (как того требует ТЗ)
    // ---
    // лучшим решением было бы здесь вызвать thread_pool.join(), с тем чтобы главный поток был
    // занят делом, а не "просто крутился вхолостую", но всё же как сказано в ТЗ, так и поступим...
    using namespace std::chrono_literals;
    int cmd_num = 1;
    while (!g_terminated)
    {
      std::this_thread::sleep_for(250ms);
      // подготавливаем команду
      std::string cmd = "cmd" + std::to_string(cmd_num++);
      logger->log(cmd.c_str());
      homework25::receive(generator1, cmd);
      //commands.push_command(new homework25::my_command_t(&thread_pool));
    }

    // удалям контекст, который готовил команды
    homework25::disconnect(generator1);

    // после того, как пользователь нажал кнопки Ctrl+C, - дожидаемся завершения работы всех потоков
    writer2->join();
    writer1->join();

    // созданные экземпляры потоков и <thread pool> будет уничтожен scoped-деструкторами
  }

#if 0
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
#endif

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


static homework25::abstract_logger_t* g_logger = nullptr;
#ifdef CPP_OTUS_WINDOWS
#include <Windows.h>
BOOL WINAPI console_handler(DWORD signal)
{
  if (signal == CTRL_C_EVENT)
  {
    g_logger->log("Ctrl+C pressed & handled...");
    g_terminated = true;
    return TRUE;
  }
  return FALSE;
}
#elif defined(CPP_OTUS_POSIX)
#include <signal.h>
void console_ctrlc_handler(int /*sig*/)
{
  g_logger->log("Ctrl+C pressed & handled...");
  g_terminated = true;
}
#endif

void init_terminate_handler(homework25::abstract_logger_t* logger)
{
  g_logger = logger;
#ifdef CPP_OTUS_WINDOWS
  SetConsoleCtrlHandler(console_handler, TRUE);
#elif defined(CPP_OTUS_POSIX)
  signal(SIGINT, console_ctrlc_handler);
  signal(SIGQUIT, console_ctrlc_handler);
  signal(SIGTERM, console_ctrlc_handler);
  //не перехватывается:signal(SIGKILL, console_ctrlc_handler);
  signal(SIGABRT, core::stack_trace_handler);
  signal(SIGSEGV, core::stack_trace_handler);
#endif
}


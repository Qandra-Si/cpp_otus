// -*- mode: c++; coding: utf-8 -*-

#include <thread>
#include <chrono>
#include <mutex>
#include <string>
#include <list>

#include <core/waiter.h>

#include "logger.h"


namespace homework25
{

//------------------------------------------------------------------------------
// logger_t::impl_t
//------------------------------------------------------------------------------
class logger_t::impl_t : public abstract_logger_t
{
  using lines_t = std::list<std::string>;
public:
  impl_t(std::ostream& terminal) : m_terminated(false), m_terminal(terminal)
  {
    // запускаем потоки
    m_thread = std::thread(&impl_t::do_run, this);
    // ждём пока impl_t не будет готов
    m_wait_for_ready.wait();
  }

  ~impl_t()
  {
    // останавливаем работу потока (контролируемый останов)
    m_terminated = true;
    if (m_thread.joinable())
    {
      if (m_thread.get_id() == std::this_thread::get_id())
        m_thread.detach();
      else
        m_thread.join();
    }
    // выводим на экран все сообщения, которые ещё хранятся в списке
    print_lines(m_lines);
  }

  virtual void log(const char* line) override
  {
    std::scoped_lock<std::mutex> locker(m_lines_guard);
    m_lines.emplace_back(std::string(line));
  }

private:
  void do_run()
  {
    // ДОЛЖЕН ИДТИ ПЕРВЫМ:
    // настраиваем отложенное пробуждение waiter-а в случае завершения работы метода (чтобы случайно не забыть)
    core::scoped_wake_t wakeup_on_return(m_wait_for_ready);
    // настраиваем работу тела потока
    using namespace std::chrono_literals;
    lines_t tmp;
    // выставляем признак готовности потока к работе (уже запущен и уже проинициализирован)
    wakeup_on_return.wake();

    // вечный цикл, пока работа потока не будет прервана снаружи
    while (!m_terminated)
    {
      // пытаемся получить сообщения для вывода в терминал
      {
        tmp.clear();
        std::scoped_lock<std::mutex> locker(m_lines_guard);
        tmp.swap(m_lines);
      }
      // если новых сообщений нет, то бездействуем указанное кол-во времени
      if (tmp.empty())
      {
        std::this_thread::sleep_for(10ms);
        continue;
      }
      // выводим не экран все накопившиеся сообщения
      print_lines(tmp);
      std::this_thread::yield();
    }
  }

  void print_lines(const lines_t& lines)
  {
    for (const auto& line : lines)
      m_terminal << line << '\n';
    m_terminal << std::flush;
  }

private:
  // сигнал о том, что поток должен прекратить работу
  volatile bool m_terminated;
  // поток в котором запускается логгер
  std::thread m_thread;
  // сигнал о том, что поток проинициализировался и работает
  core::waiter_t m_wait_for_ready;

  // reference на поток терминала, куда следует выводить сообщения
  std::ostream& m_terminal;
  // сообщения, которые поток должен вывести на экран (в терминал) + guard для них
  std::mutex m_lines_guard;
  lines_t m_lines;
};

//------------------------------------------------------------------------------
// logger_t
//------------------------------------------------------------------------------
logger_t::logger_t(std::ostream& terminal) : p_impl(new impl_t(terminal))
{
}

logger_t::~logger_t()
{
  delete p_impl;
}

abstract_logger_t* logger_t::get_interface()
{
  return p_impl;
}

}
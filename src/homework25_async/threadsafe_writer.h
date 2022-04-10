// -*- mode: c++; coding: utf-8 -*-
/*! \file threadsafe_writer.h
* \brief Потокобезопасный writer домашнего задания 25ой лекции
*/

#pragma once

#include "leaders_followers.h"



/*! \addtogroup homework25
* @{
*/

namespace homework25 {

/*! \brief Менеджер (запускатель) потоков для асинхронной обработки команд
*
* Класс, который делает spawn потоков (любого количества), подключая созданные потоки к
* <thread pool> экземпляру с тем, чтобы пул потоков начал выбирать <leader> и <follower>
* потоки, которые получая данные из <handle set>, будут выполнять операции по выполнению
* поступивших команд.
*
* Это упрощённая реализация join() к экземпляру <thread pool>, которая не поддерживает
* INFINITE операции, т.о. не поддерживается timeout_ms=0 и <follower> потоки постоянно
* крутятся в повторных вызовах join() методов. Для полнофункциональной поддержки данного
* функционала, надо перейти на использование select() операций и сложной и тонкой настройки
* останова потоков... в данном случае обойдёмся циклом с не очень длинным дискретом ожидания.
*/
class thread_manager_t
{
public:
  thread_manager_t(leaders_followers::thread_pool_t* tp, volatile bool& terminated) : m_thread_pool(tp), m_terminated(terminated) { }

  using thread_t = std::shared_ptr<std::thread>;
  thread_t spawn()
  {
    auto join = [](leaders_followers::thread_pool_t* tp, volatile bool* terminated) { while (!terminated) tp->join(100); };
    return thread_t(new std::thread(join, m_thread_pool, &m_terminated));
  }

private:
  thread_manager_t() = delete; // отключаем конструктор по умолчанию
  thread_manager_t(const thread_manager_t&) = delete; // отключаем конструктор копирования
  thread_manager_t& operator=(const thread_manager_t&) = delete; // отключаем оператор копирования

  leaders_followers::thread_pool_t* m_thread_pool;
  volatile bool& m_terminated;
};

/*! \brief Application-specific команда, которая асинхронно обрабатывается во writer-потоках
*/
class my_command_t : protected leaders_followers::concrete_event_handler_t, public leaders_followers::event_handler_t
{
public:
  my_command_t(leaders_followers::thread_pool_t* tp) : leaders_followers::event_handler_t(this, tp) { }

protected:
  // методы deactivate_handle и reactivate_handle не используется, см. описание
  // в классе my_set_of_commands_t
  virtual leaders_followers::handle_t* get_handle() override { return nullptr; }

  // это точка входа writer-а, ктоорый был вызван асинхронно одним из потоков <thread pool>
  virtual void handle() override;

private:
  my_command_t() = delete; // отключаем конструктор по умолчанию
  my_command_t(const my_command_t&) = delete; // отключаем конструктор копирования
  my_command_t& operator=(const my_command_t&) = delete; // отключаем оператор копирования
};

/*! \brief Обработчик команд и по совместительству список асинхронных команд
*
* Центральный метод экземпляра этого класса, это \ref wait_for_handle. Этот метод
* может быть вызван только <leader> потоком из <thread pool>. Никакой другой
* <follower> поток не пользуется этим методом. Метод выполняет ожидание поступления
* событий в очередь команд <handle set> и как только получает (одну или несколько)
* команду, перестаёт быть <leader> потоком, меняет роль на <processor> поток, а в
* это время любой из ожидающих своей очереди <follower> потоков занимает его место
* и становится <leader> потоком. Если свободных <follower> потоков нет, например все
* заняты, то первый освободившийся <processor> поток стазу становится <leader>
* потоком и всё повторяется сначала.
*
* Методами \ref deactivate_handle и \ref reactivate_handle не пользуемся. Они
* должны занимать и/или освобождать ресурсы сервера/компьютера. Например, эти методы
* могут пометить tcp-сокет занятым до тех пор, пока не будет закончена обработка
* события/команды. Сами по себе эти методы могли бы также быть методами синхронизации
* для доступа к общему ресурсу. Но в данном примере они лишь демонстрируют логику
* leader-follower шаблона проектирования, которой я придерживаюсь в данной реализации.
*/
class my_set_of_commands_t : public leaders_followers::handle_set_t
{
public:
  my_set_of_commands_t(volatile bool& terminated);
  virtual ~my_set_of_commands_t();
  virtual void deactivate_handle(leaders_followers::handle_t*) override { }
  virtual void reactivate_handle(leaders_followers::handle_t*) override { }
  virtual bool wait_for_handle() override;
  void push_command(my_command_t* command);
  bool pop_handle();

private:
  my_set_of_commands_t() = delete; // отключаем конструктор по умолчанию
  my_set_of_commands_t(const my_set_of_commands_t&) = delete; // отключаем конструктор копирования
  my_set_of_commands_t& operator=(const my_set_of_commands_t&) = delete; // отключаем оператор копирования

  // скрытая внутреняя реализация (д.б. объявлен последним)
  class impl_t;
  impl_t* p_impl;
};

}

/*! @} */

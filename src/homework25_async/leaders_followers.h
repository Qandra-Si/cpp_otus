// -*- mode: c++; coding: utf-8 -*-
/*! \file leaders_followers.h
* \brief Управление пулом потоков домашнего задания 25ой лекции
*/

#pragma once

#include <mutex>
#include <thread>
#include <core/waiter.h>




/*! \addtogroup homework25
* @{
*/

namespace leaders_followers
{

class handle_t;

/*! \brief Интерфейс к набору команд (ресурсов), которые обрабатываются потоками пула \ref thread_pool_t
*/
class handle_set_t
{
public:
  /*! \brief Виртуальный деструктор
  * 
  * Добавляем виртуальный деструктор в таблицу виртуальных методов, чтобы
  * безопасно создавать потомки данного класса.
  */
  virtual ~handle_set_t() = default;

  /*! \brief Временная деактивация \ref handle_t во внутреннем <handle set>
  */
  virtual void deactivate_handle(handle_t* h) = 0;

  /*! \brief Реактивация деактивированного \ref handle_t во внутреннем <handle set>
  */
  virtual void reactivate_handle(handle_t* h) = 0;

  /*! \brief Ожидание поступления команды
  *
  * После того, как <follower> становится на место <leader>, он выполняет
  * этот метод и ждёт новых <handle> которые примется обрабатывать,
  * находясь в <processing> состоянии, заблаговременно передав роль
  * <follower> другому потоку
  */
  virtual bool wait_for_handle() = 0;
};

/*! \brief Паттерн для пула потоков, работающий по принципу leader-followers
*/
class thread_pool_t
{
public:
  thread_pool_t(handle_set_t* reactor);
  virtual ~thread_pool_t() = default;

  /*! \brief Подключение потока к пулу
  *
  * К пулу может быть подключен любой поток, вызвавший метод join. Когда потоку
  * будет передано управление, т.е. когда он заёмет место <leader>, то в его
  * контексте будет вызваны методы <reactor>.
  */
  void join(int timeout_ms = 0);

  /*! \brief Выбор (назначение) <follower> потока новым <leader>
  */
  void promote_new_leader();

  // поддержка функционала деактивации событий и связанных с ними ресурсов
  void deactivate_handle(handle_t* h) { m_reactor->deactivate_handle(h); }
  void reactivate_handle(handle_t* h) { m_reactor->reactivate_handle(h); }

private:
  /*! \brief Диспетчер событий и доступности ресурсов
  */
  handle_set_t* m_reactor;

  /*! \brief Идентификатор thread-а, который в настоящее время является leader-ом
  *
  * В противном случае 0 означает NO_LEADER_THREAD, т.е. отсутствие leader-а в пуле.
  */
  std::thread::id m_leader_thread;

  /*! \brief Follower-ы пула ждут эту conditional-переменную для того, чтобы стать leader-ами
  */
  core::waiter_t m_followers_condition;

  /*! \brief Синхронизация доступа к внутреннему состоянию пула
  */
  std::timed_mutex m_mutex;
};

/*! \brief Декоратор \ref event_handler_t для application-specific обработчиков событий
*/
class concrete_event_handler_t
{
public:
  /*! \brief Виртуальный деструктор
  *
  * Добавляем виртуальный деструктор в таблицу виртуальных методов, чтобы
  * безопасно создавать потомки данного класса.
  */
  virtual ~concrete_event_handler_t() = default;

  /*! \brief Дескриптор события/обработчика, который использует reactivator
  */
  virtual handle_t* get_handle() = 0;

  /*! \brief Application-specific метод для асинхронной обработки события/команды
  *
  * Ради этого метода всё здесь и затевалось!
  */
  virtual void handle() = 0;
};

/*! \brief Обработчик события, которое попало <leader> потоку в \ref thread_pool_t
*/
class event_handler_t
{
public:
  event_handler_t(concrete_event_handler_t* eh, thread_pool_t* tp);
  event_handler_t& operator=(const event_handler_t& other);

  void handle_event();

private:
  /*! \brief Декоратор над \ref concrete_event_handler_t
  *
  * Используется для того, чтобы приложение могло использовать любой custom-обработчик.
  */
  concrete_event_handler_t* m_concrete_event_handler;

  /*! \brief Экземпляр пула потоков
  */
  thread_pool_t* m_thread_pool;
};

}

/*! @} */

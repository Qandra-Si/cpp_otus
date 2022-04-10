// -*- mode: c++; coding: utf-8 -*-

#include <list>
#include <memory>
#include <chrono>

#include "leaders_followers.h"


//------------------------------------------------------------------------------
// leaders_followers
//------------------------------------------------------------------------------
namespace leaders_followers
{

static const std::thread::id NO_CURRENT_LEADER{};

//------------------------------------------------------------------------------
// thread_pool_t
//------------------------------------------------------------------------------
thread_pool_t::thread_pool_t(handle_set_t* reactor) : m_reactor(reactor), m_followers_condition(false)
{
}

void thread_pool_t::join(int timeout_ms)
{
  // используем scoped-блокировку для захвата мьютекса
  if (!m_mutex.try_lock_for(std::chrono::milliseconds(timeout_ms))) return;

  while(true)
  {
    while (m_leader_thread != NO_CURRENT_LEADER)
    {
      // засыпаем и отсвобождаем <mutex> автоматически
      if (!m_followers_condition.wait(timeout_ms)) { m_mutex.unlock(); return; }
    }

    // получаем роль leader-а
    m_leader_thread = std::this_thread::get_id();
    // временно покидаем мониторинг мьютекса и разрешаем другим follower-ам
    // выполнить join к пулу
    m_mutex.unlock();

    // после того, как данный поток стал leader-ом, он должен воспользоваться
    // reactor-ом для ожидания поступления события
    if (!m_reactor->wait_for_handle()) return;

    // повторно приступить к мониторингу мьютекса, пока leader-поток
    // крутится в вечном цикле выше
    m_mutex.lock();
  }
}

void thread_pool_t::promote_new_leader()
{
  // используем scoped-блокировку для захвата мьютекса
  m_mutex.lock();

  // проверяем ошибочную ситуацию, только leader может вызывать этот метод
  if (m_leader_thread == std::this_thread::get_id())
  {
    // выставляем флаг, что leader-а в пуле больше нет и уведомляем
    // метод join, чтобы он выбрал нового leader-а
    m_leader_thread = NO_CURRENT_LEADER;
    m_followers_condition.wake();
  }

  m_mutex.unlock();
}

//------------------------------------------------------------------------------
// event_handler_t
//------------------------------------------------------------------------------
event_handler_t::event_handler_t(concrete_event_handler_t* eh, thread_pool_t* tp) :
  m_concrete_event_handler(eh),
  m_thread_pool(tp)
{
}

event_handler_t& event_handler_t::operator=(const event_handler_t& other)
{
  m_concrete_event_handler = other.m_concrete_event_handler;
  m_thread_pool = other.m_thread_pool;
  return *this;
}

void event_handler_t::handle_event()
{
  // временно деактивируем handler в reactor-е для предотвращения гонок,
  // т.е. для предотвращения одновременного доступа к разными потоками в одному
  // и тому-же событию/ресурсу
  m_thread_pool->deactivate_handle(m_concrete_event_handler->get_handle());

  // назначить ожидающего follower-а новым leader-ом
  m_thread_pool->promote_new_leader();

  // выполнить application-specific действия по обработке события
  m_concrete_event_handler->handle();

  // реактивировать ранее выключенное событие/ресурс
  m_thread_pool->reactivate_handle(m_concrete_event_handler->get_handle());
}

}
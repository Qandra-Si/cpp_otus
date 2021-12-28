// -*- mode: c++; coding: utf-8 -*-

#include <core/waiter.h>
#include <cassert>
#if defined(CPP_OTUS_WINDOWS)
#  include <Windows.h>
#else
#  include <condition_variable>
#  include <thread>
#endif
#include <chrono>


namespace core
{

#if defined(CPP_OTUS_WINDOWS) // Win32 API

waiter_t::waiter_t(bool manual_reset)
{
  m_win32_event = ::CreateEvent(NULL, manual_reset, false, NULL);
  assert(m_win32_event);
}

waiter_t::~waiter_t()
{
  ::CloseHandle(m_win32_event);
}

bool waiter_t::wait(int timeout_ms)
{
  //здесь нельзя сбрасывать signaling state (другой поток его может уже установить ДО того, как вызван wait)
  // см. http://pm.asoft:8080/redmine/issues/4775
  //reset();
  DWORD result = ::WaitForSingleObject(m_win32_event, timeout_ms ? timeout_ms : INFINITE); // 3
  assert( (result == WAIT_TIMEOUT || result == WAIT_OBJECT_0) );
  return result != WAIT_TIMEOUT;
  // ВНИМАНИЕ! при значении флага manual_reset=false сигнальное состояние будет сброшено АВТОМАТИЧЕСКИ
}

void waiter_t::wake()
{
  SetEvent(m_win32_event); // 2
}

void waiter_t::reset()
{
  ::ResetEvent(m_win32_event);
}

#else  // condition_varibale

waiter_t::waiter_t(bool manual_reset) : m_awoken(false), m_manual_reset(manual_reset)
{
}

waiter::~waiter()
{
}

bool waiter::wait(int timeout_ms)
{
  try
  {
    std::unique_lock<std::mutex> lock(m_mutex);

    if (m_awoken)
    {
      if (!m_manual_reset)
        m_awoken = false;
      return true;
    }

    if (timeout_ms == 0)
    {
      m_cond_var.wait(lock, [this]{return m_awoken;});
      if (!m_manual_reset)
        m_awoken = false;
      return true;
    }

    const bool waked_up = m_cond_var.wait_for(lock, std::chrono::milliseconds(timeout_ms), [this]{return m_awoken;});
    if (waked_up)
    {
      if (!m_manual_reset)
        m_awoken = false;
    }
    return waked_up;
  }
  catch(...)
  {
    // Если мы здесь все плохо... совсем все плохо с потоками в stl ?
    assert(0);
    throw;
  }
  return false;
}

void waiter::wake()
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_awoken = true;
  m_cond_var.notify_one();
}

void waiter::reset()
{
  std::lock_guard<std::mutex> lock(m_mutex);
  m_awoken = false;
}

#endif

}

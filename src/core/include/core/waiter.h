// -*- mode: c++; coding: utf-8 -*-
/*! \file waiter.h
 * \brief Кастомная реализация паттерна "guarded suspension"
*/
#pragma once

#include <core/core.h>

#if !defined(CPP_OTUS_WINDOWS)
#  include <mutex>
#  include <condition_variable>
#endif


/*! \addtogroup core
 * @{
*/
namespace core {

/*! \brief Блокировка потока в ожидание некого события
*
* По своей сути класс является инкапусляпцией связки mutex + condition_variable
*
* Основные замечения по реализации:
* \li \c В реализаиции boost::condition_variable для Win32 - очень большой оверхед
* \li \c Реализация boost::condition_variable для Win32 (версия 1.46.1, а может быть и позднее) - ведет себя некоректно (что приводит к "падению" программы)
*
* Поэтому в случае Win32 используются напрямую ф-ции WinApi: SetEvent/WaitForSingleObject и т.п.
*
* \note Все методы Thread-Safe
*/
class waiter_t
{
public:
  /*! \brief Конструктор "guarded suspension" сигнализатора
  * \param manual_reset Признак автоматического/ручного перевода объекта в signalling state после ожидания
  *
  * Если флаг \ref manual_reset равен true, то создаётся экземпляр объекта, который требует
  * обязательного использования метода \reset для перевода объекта в nonsignalled состояние.
  * Если флаг \ref manual_reset равен false (по умолчанию), то создаётся экземпляр объекта
  * с автосбросом состояния в nonsignalled после использования метода \wait.
  */
  waiter_t(bool manual_reset = false);

  ~waiter_t();

public:
  /*! \brief Ждать сигнала
  * \param timeout_ms Маскимальное время ожидания в мс, если timeout_ms == 0, то время ожидания бесконечно
  * \return true если объект был разбужен вызовом методом \ref wake, если же был таймаут ожидания, то возвращает \e false
  *
  * Ф-ция блокирует выполнение текущего потока в ожидание, пока пользователь не вызовет метод \ref wake.
  *
  * \warning Если до вызова \ref wait объект был разбужен (один или несколько раз) вызовом \ref wake, то
  *          \ref wait вернет \b true. Т.е. поведение аналогично WaitForSingleObject, в котором в отличие
  *          от condition_variable проверяется "signalling state" прежде чем заснуть.
  *
  * \sa wake
  */
  bool wait(int timeout_ms = 0);

  /*! \brief Сигнализировать объекту о завершение ожидания (выставить signalling state)
  */
  void wake();

  /*! \brief Принудительно сбросить signalling state у объекта
  */
  void reset();

private:
  waiter_t(const waiter_t&) = delete; // отключаем конструктор копирования
  const waiter_t& operator=(const waiter_t&) = delete; // отключаем оператор копирования

#if defined(CPP_OTUS_WINDOWS)
  void* m_win32_event;
#else // condition_variable and mutex implementation
  std::mutex m_mutex;
  std::condition_variable m_cond_var;
  bool m_awoken; // flag signaling state
  const bool m_manual_reset;
#endif
};

/*! \brief Автопробуждающий waiter
*/
class scoped_wake_t
{
public:
  inline scoped_wake_t(waiter_t& wait_for) : m_waiter(wait_for), m_awoken(false) { }
  inline ~scoped_wake_t() { wake(); }
  inline void wake() { if (m_awoken) return; m_waiter.wake(); m_awoken = true; }
  inline void release() { m_awoken = true; }
private:
  scoped_wake_t(const scoped_wake_t&) = delete; // отключаем конструктор копирования
  void operator=(const scoped_wake_t&) = delete; // отключаем оператор копирования
  core::waiter_t& m_waiter;
  bool m_awoken;
};

}

/*! @} */

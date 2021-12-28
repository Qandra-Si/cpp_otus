// -*- mode: c++; coding: utf-8 -*-
// Unit-тесты для core/waiter.h

#include <gtest/gtest.h>
#include <gtest/internal/gtest-internal.h>
#include <thread>

#include <core/waiter.h>

using namespace std::chrono_literals;


/******************************************************************************
   test_waiter_wait
*******************************************************************************/

TEST(test_waiter, test_waiter_wait )
{
  // здесь проверяется, что метод wait() генерирует задержку в 100 мс,
  // т.к. сразу после создания объект находится в non signalling state
  core::waiter_t w;
  auto t1 = std::chrono::high_resolution_clock::now();
  bool waked_up = w.wait(100); 
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> elapsed = t2 - t1;

  ASSERT_FALSE( waked_up ); // объект не был разбужен
  ASSERT_TRUE( elapsed.count() > 50 );
}

/******************************************************************************
   test_waiter_wake
*******************************************************************************/

TEST(test_waiter, test_waiter_wake )
{
  // здесь проверяется, что метод wait() НЕ генерирует задержку в 1 с,
  // т.к. объект был переведён в signalling state, поэтому wake() проходим
  // без задержек
  core::waiter_t w;
  w.wake();
  auto t1 = std::chrono::high_resolution_clock::now();
  bool waked_up = w.wait(1000);
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> elapsed = t2 - t1;

  ASSERT_TRUE( waked_up ); // объект был разбужен
  ASSERT_TRUE( elapsed.count() < 50 );
}

/******************************************************************************
   test_waiter_reset
*******************************************************************************/

TEST(test_waiter, test_waiter_reset)
{
  // здесь проверяется, что метод wait() генерирует задержку в 100 мс,
  // т.к. объект принудительно сброшен в non signalling state
  core::waiter_t w;
  w.wake();
  w.reset();
  auto t1 = std::chrono::high_resolution_clock::now();
  bool waked_up = w.wait(100);
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> elapsed = t2 - t1;

  ASSERT_FALSE( waked_up ); // объект не был разбужен
  ASSERT_TRUE( elapsed.count() > 50 );
}

/******************************************************************************
   test_waiter_check_sequence
*******************************************************************************/

class waiter_thread
{
public:
  std::thread *m_thread;
  core::waiter_t m_waiter;
  waiter_thread()
  {
    m_thread = new std::thread( &waiter_thread::run, this );
  }
  virtual ~waiter_thread()
  {
    m_thread->join();
    delete m_thread;
  }
  void run()
  {
    std::this_thread::sleep_for(1000ms);
    m_waiter.wake();
  }
};

TEST(test_waiter, test_waiter_check_sequence)
{
  // здесь проверяется, что когда объект находится в signalling state, то
  // совершенно не важно сколько времени пройдёт до wait()

  // поэтому проверять будем так:
  //  вариант (1) - сначала вызываем wake(), потом wait()
  //  вариант (2) - сначала вызываем wait(), потом в потоке wake()

  core::waiter_t w;
  bool waked_up = w.wait(50); // проверка: тест не должен зависнуть, даже если waiter не работает
  ASSERT_FALSE( waked_up ); // проверка: убеждаемся, что объект не был разбужен (важно для следующей проверки)
  w.wake();
  waked_up = w.wait(50);
  ASSERT_TRUE( waked_up ); // (1) объект был разбужен

  waiter_thread t;
  waked_up = t.m_waiter.wait(3000); // объект должен быть разбужен, а если нет то это ошибка (но тест не должен зависнуть)
  ASSERT_TRUE( waked_up ); // (2) объект был разбужен
}

/******************************************************************************
test_waiter_automatic_reset
*******************************************************************************/

TEST(test_waiter, test_waiter_automatic_reset)
{
  // здесь проверяется, что метод wait() АВТОМАТИЧЕСКИ переводит объект в nonsignalling состояние
  core::waiter_t w;

  bool zero_waked_up = w.wait(50);
  ASSERT_FALSE(zero_waked_up); // объект пока ещё не разбужен

  w.wake();

  bool first_waked_up = w.wait(50);
  ASSERT_TRUE(first_waked_up); // объект был разбужен

  bool second_waked_up = w.wait(50);
  ASSERT_FALSE(second_waked_up); // объект АВТОМАТИЧЕСКИ переведён в nonsignalling состояние и НЕ разбужен

  w.reset();

  bool third_waked_up = w.wait(50);
  ASSERT_FALSE(third_waked_up); // объект всё ещё переведён в nonsignalling состояние
}

/******************************************************************************
test_waiter_manual_reset
*******************************************************************************/

TEST(test_waiter, test_waiter_manual_reset)
{
  // здесь проверяется, что метод wait() НЕ переводит объект в nonsignalling состояние
  core::waiter_t w(true);

  bool zero_waked_up = w.wait(50);
  ASSERT_FALSE(zero_waked_up); // объект пока ещё не разбужен

  w.wake();

  bool first_waked_up = w.wait(50);
  ASSERT_TRUE(first_waked_up); // объект был разбужен

  bool second_waked_up = w.wait(50);
  ASSERT_TRUE(second_waked_up); // объект всё ещё разбужен

  w.reset();

  bool third_waked_up = w.wait(50);
  ASSERT_FALSE(third_waked_up); // объект переведён в nonsignalling состояние
}

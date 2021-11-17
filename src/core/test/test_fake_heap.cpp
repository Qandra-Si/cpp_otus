// -*- mode: c++; coding: utf-8 -*-
// Unit-тесты для core/utils.h

#include <gtest/gtest.h>
#include <gtest/internal/gtest-internal.h>

#include <core/fake_heap.h>
#include <core/fake_heap_helpers.h>

#include <new>
#include <array>
#include <iostream>


class debugger_t : public core::fake_heap_debugger_t
{
public:
  virtual std::basic_ostream<char>& get() override { return std::cout; }
} debugger;



/******************************************************************************
   basic
*******************************************************************************/

TEST(test_fake_heap, basic)
{
  core::fake_heap_t fh(1024);
  void* p;

  ASSERT_NO_THROW(p = fh.malloc(1));
  ASSERT_NE(nullptr, p);
  ASSERT_NO_THROW(fh.free(p));
}

/******************************************************************************
   debug
*******************************************************************************/

TEST(test_fake_heap, debug)
{
  core::fake_heap_t fh(1024, &debugger);
  void* p;

  ASSERT_NO_THROW(p = fh.malloc(1));
  ASSERT_NE(nullptr, p);
  ASSERT_NO_THROW(fh.free(p));
}

/******************************************************************************
   malloc
*******************************************************************************/

TEST(test_fake_heap, malloc)
{
  std::array<void*, 8> pp;
  constexpr std::size_t max_size = pp.size() * 16;
  core::fake_heap_t fh(max_size, &debugger);

  // занимаем всю память
  for (unsigned i = 0; i < pp.size(); ++i)
  {
	ASSERT_NO_THROW(pp[i] = fh.malloc(16));
	ASSERT_NE(nullptr, pp[i]);
  }
  // освобождаем всю память
  for (unsigned i = 0; i < pp.size(); ++i)
  {
	ASSERT_NO_THROW(fh.free(pp[i]));
  }

  // поскольку память кончилась, а сборщика мусора нет, как и перевода её в
  // вакантное состояние, то новый malloc окажется неудачным
  EXPECT_THROW(fh.malloc(1), std::bad_alloc);

  // после сброса фейковой кучи выделение чанка снова становится доступным
  fh.reset();

  // после reset-а можно вновь занять память
  ASSERT_NO_THROW(fh.malloc(max_size));
  EXPECT_THROW(fh.malloc(1), std::bad_alloc);

  void* p;

  // переводим фейковую кучу в safe_mode режим (будет использоваться куча cstdlib)
  fh.enable_safe_mode();

  ASSERT_NO_THROW(p = fh.malloc(1));
  ASSERT_NE(nullptr, p);
  ASSERT_NO_THROW(fh.free(p));

  // отключаем safe_mode режим и убеждаемся, что память снова не выделяется
  fh.disable_safe_mode();

  EXPECT_THROW(fh.malloc(1), std::bad_alloc);
}

/******************************************************************************
   calloc
*******************************************************************************/

TEST(test_fake_heap, calloc)
{
  const void* fourteen_zeros = "\0\0\0\0\0\0\0\0\0\0\0\0\0";
  void* p;
  core::fake_heap_t fh(14, &debugger);

  // занимаем всю веделенную память и проверяем, что она занулена
  ASSERT_NO_THROW(p = fh.calloc(1, 14));
  ASSERT_NE(nullptr, p);
  ASSERT_EQ(0, memcmp(fourteen_zeros, p, 14));

  // оставляем след в памяти
  strncpy(reinterpret_cast<char*>(p), "Hello, World!", 14);

  // поскольку память кончилась, а сборщика мусора нет, как и перевода её в
  // вакантное состояние, то новый calloc окажется неудачным
  EXPECT_THROW(fh.calloc(1, 1), std::bad_alloc);

  // сбрасываем фейковую кучу, выделение снова становится доступным
  fh.reset();

  // после reset-а можно вновь занять память
  // прочеряем, что она снова занулена
  ASSERT_NO_THROW(p = fh.calloc(2, 7));
  ASSERT_NE(nullptr, p);
  ASSERT_EQ(0, memcmp(fourteen_zeros, p, 14));
  ASSERT_NO_THROW(fh.free(p));

  // переводим фейковую кучу в safe_mode режим (будет использоваться куча cstdlib)
  fh.enable_safe_mode();

  ASSERT_NO_THROW(p = fh.calloc(7, 2));
  ASSERT_NE(nullptr, p);
  ASSERT_EQ(0, memcmp(fourteen_zeros, p, 14));
  ASSERT_NO_THROW(fh.free(p));

  // отключаем safe_mode режим и убеждаемся, что память снова не выделяется
  fh.disable_safe_mode();

  EXPECT_THROW(fh.calloc(1, 1), std::bad_alloc);
}

/******************************************************************************
   single_chunk
*******************************************************************************/

TEST(test_fake_heap, single_chunk)
{
  core::fake_realloc_heap_t fh(4+128, 128, &debugger);
  void *p0, *p1, *p2, *p3;

  // выделяем место в единственном чанке
  ASSERT_NO_THROW(p0 = fh.malloc(6));
  ASSERT_NE(nullptr, p0);
  strncpy(reinterpret_cast<char*>(p0), "Hello", 6);

  // убеждаемся, что новый чанк не выделяется
  EXPECT_THROW(p1 = fh.malloc(1), std::bad_alloc);

  // убеждаемся, что новый чанк будет недоступен и после освобождения первого
  ASSERT_NO_THROW(fh.free(p0));
  EXPECT_THROW(p1 = fh.malloc(1), std::bad_alloc);

  // после сброса фейковой кучи выделение чанка снова становится доступным
  fh.reset();
  ASSERT_NO_THROW(p1 = fh.malloc(6));
  // и указатель на новый блок памяти равен ранее возвращённому
  ASSERT_EQ(p0, p1);
  // и в куче остались данные, ранее скопированные туда
  ASSERT_STREQ("Hello", reinterpret_cast<char*>(p1));

  // убеждаемся, что realloc в пределах чанка и той же самой кучи доступен
  // при этом указатель после реаллокации не передвинулся
  ASSERT_NO_THROW(p2 = fh.realloc(p1, 14));
  ASSERT_EQ(p1, p2);
  // и в чанке продолжают присутствовать ранее скопированные данные
  ASSERT_STREQ("Hello", reinterpret_cast<char*>(p2));

  strncpy(reinterpret_cast<char*>(p2), "Hello, World!", 14);

  // уменьшать блок памяти с помощью реаллокации также можно
  ASSERT_NO_THROW(p3 = fh.realloc(p2, 6));
  ASSERT_EQ(p2, p3);

  // реаллоцировать в предельный размер чанка также можно
  ASSERT_NO_THROW(p3 = fh.realloc(p2, 128));

  // а вот разместить данных в куче больше чем один чанк нельзя
  EXPECT_THROW(p3 = fh.realloc(p2, 129), std::bad_alloc);

  // переводим фейковую кучу в safe_mode режим (будет использоваться куча cstdlib)
  fh.enable_safe_mode();

  // теперь стало доступно перемещение данных из фейковой кучи в cstdlib-кучу
  ASSERT_NO_THROW(p3 = fh.realloc(p2, 129));
  ASSERT_NE(p2, p3);
  // и в cstdlib-кучу перемещены ранее проинициализированные данные
  ASSERT_STREQ("Hello, World!", reinterpret_cast<char*>(p3));
  ASSERT_NO_THROW(fh.free(p3));

  // отключаем safe_mode режим и убеждаемся, что память снова не выделяется
  fh.disable_safe_mode();

  EXPECT_THROW(fh.calloc(1, 1), std::bad_alloc);
}

/******************************************************************************
   realloc
*******************************************************************************/

TEST(test_fake_heap, realloc)
{
	constexpr const int chunk_size = 8;
	core::fake_realloc_heap_t fh(5 * (4+chunk_size), chunk_size, &debugger);
	void* p0, * p1, * p2, * p4;

	// выделяем место в первом чанке
	ASSERT_NO_THROW(p0 = fh.malloc(6));
	ASSERT_NE(nullptr, p0);
	strncpy(reinterpret_cast<char*>(p0), "Hello", 6);

	// занимаем место во втором чанке (максимальная длина пользовательской области: chunk_size-4)
	ASSERT_NO_THROW(p1 = fh.malloc(chunk_size));
	ASSERT_NE(nullptr, p1);

	// первому чанку расшириться во второй не получится, поэтому он должен быть
	// перенесён в третий и расшириться в четвёртый
	// убеждаемся, что realloc в пределах той же самой кучи доступен
	// при этом указатель после реаллокации передвинулся
	ASSERT_NO_THROW(p2 = fh.realloc(p0, 14));
	ASSERT_NE(p0, p2);
	// и в новом чанке присутствуют ранее скопированные в первый чанк данные
	ASSERT_STREQ("Hello", reinterpret_cast<char*>(p2));

	strncpy(reinterpret_cast<char*>(p2), "Hello, World!", 14);

	// запрешиваем очередонй realloc, который приведёт к расширению блока данных в следующий chunk
	// старые указатели должны остаться на тех же местах
	ASSERT_NO_THROW(p4 = fh.realloc(p2, 17));
	ASSERT_EQ(p2, p4);
	// во втором, третьем и четвёртом чанках присутствуют ранее скопированные данные
	ASSERT_STREQ("Hello, World!", reinterpret_cast<char*>(p4));

	// убеждаемся, что память более не выделяется (все чанки заняты)
	EXPECT_THROW(fh.calloc(1, 1), std::bad_alloc);
}

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
  проверка malloc

  === > fake_heap.reset from fake:00BD9AD8 to fake:00BD9B58
  === > fake_heap.malloc from fake:00BD9AD8 to fake:00BD9AE8, size 16
  === > fake_heap.malloc from fake:00BD9AE8 to fake:00BD9AF8, size 16
  === > fake_heap.malloc from fake:00BD9AF8 to fake:00BD9B08, size 16
  === > fake_heap.malloc from fake:00BD9B08 to fake:00BD9B18, size 16
  === > fake_heap.malloc from fake:00BD9B18 to fake:00BD9B28, size 16
  === > fake_heap.malloc from fake:00BD9B28 to fake:00BD9B38, size 16
  === > fake_heap.malloc from fake:00BD9B38 to fake:00BD9B48, size 16
  === > fake_heap.malloc from fake:00BD9B48 to fake:00BD9B58, size 16
  === > fake_mem.free fake:00BD9AD8
  === > fake_mem.free fake:00BD9AE8
  === > fake_mem.free fake:00BD9AF8
  === > fake_mem.free fake:00BD9B08
  === > fake_mem.free fake:00BD9B18
  === > fake_mem.free fake:00BD9B28
  === > fake_mem.free fake:00BD9B38
  === > fake_mem.free fake:00BD9B48

  === > fake_heap.reset from fake:00BD9AD8 to fake:00BD9B58
  === > fake_heap.malloc from fake:00BD9AD8 to fake:00BD9B58, size 128
  === > stdlib.malloc heap:00BE0368 size 1
  === > stdlib.free heap:00BE0368
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
  проверка calloc

  === > fake_heap.reset from fake:00BE4D60 to fake:00BE4D6E
  === > fake_heap.calloc from fake:00BE4D60 to fake:00BE4D6E, size 14

  === > fake_heap.reset from fake:00BE4D60 to fake:00BE4D6E
  === > fake_heap.calloc from fake:00BE4D60 to fake:00BE4D6E, size 14
  === > fake_mem.free fake:00BE4D60
  === > stdlib.calloc heap:00BD9AD8 size 14
  === > stdlib.free heap:00BD9AD8
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
  проверка single_chunk

  === > fake_heap.reset from fake:00BD9AD8 to fake:00BD9B5C
  === > fake_heap.malloc from fake:00BD9ADC to fake:00BD9B5C, size 4+128 (adopted from 6)
  === > fake_mem.free fake:00BD9ADC

  === > fake_heap.reset from fake:00BD9AD8 to fake:00BD9B5C
  === > fake_heap.malloc from fake:00BD9ADC to fake:00BD9B5C, size 4+128 (adopted from 6)
  === > fake_mem.realloc from fake:00BD9ADC to fake:00BD9B5C, new size 14 (stayed in chunk 128)
  === > fake_mem.realloc from fake:00BD9ADC to fake:00BD9B5C, new size 6 (stayed in chunk 128)
  === > fake_mem.realloc from fake:00BD9ADC to fake:00BD9B5C, new size 128 (stayed in chunk 128)
  === > stdlib.malloc heap:00BE9D98 size 129
  === === > fake_mem.realloc moved from fake:00BD9ADC to heap:00BE9D98, new size 129 (readopted from 128)
  === > stdlib.free heap:00BE9D98
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
  проверка realloc

  === > fake_heap.reset from fake:00BD9AD8 to fake:00BD9B14
  === > fake_heap.malloc from fake:00BD9ADC to fake:00BD9AE4, size 4+8 (adopted from 6)
  === > fake_heap.malloc from fake:00BD9AE8 to fake:00BD9AF0, size 4+8 (adopted from 8)
  === > fake_heap.malloc from fake:00BD9AF4 to fake:00BD9B04, size 4+16 (adopted from 14)
  === === > fake_mem.realloc moved from fake:00BD9ADC to fake:00BD9AF4, new size 14 (adopted to 16, readopted from 8)
  === > fake_mem.realloc from fake:00BD9AF4 to fake:00BD9B0C, new size 24 (readopted from 16)
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

// -*- mode: c++; coding: utf-8 -*-
// Unit-тесты для core/fake_heap.h

#include <gtest/gtest.h>
#include <gtest/internal/gtest-internal.h>

#include <core/fake_heap.h>
#include <core/fake_heap_helpers.h>
#include <core/fake_heap_allocator.h>

#include <new>
#include <array>
#include <iostream>
#include <type_traits>


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

/******************************************************************************
  allocator для int-а
*******************************************************************************/

TEST(test_fake_heap, int_allocator)
{
  core::fake_heap_allocator_t<int> alloc(2*sizeof(int));

  ASSERT_EQ(2, alloc.max_size());

  static_assert(std::is_same<int, decltype(alloc)::value_type>::value, "Check allocator declarations");
  int* p = alloc.allocate(1);
  ASSERT_NE(nullptr, p);
  ASSERT_NO_THROW(alloc.deallocate(p, 1));

  using traits_t = std::allocator_traits<decltype(alloc)>;
  ASSERT_NO_THROW(p = traits_t::allocate(alloc, 1));
  traits_t::construct(alloc, p, 7);
  ASSERT_EQ(7, *p);
  ASSERT_NO_THROW(traits_t::deallocate(alloc, p, 1));

  EXPECT_THROW(p = traits_t::allocate(alloc, 1), std::bad_alloc);
}

/******************************************************************************
  allocator для string-а
*******************************************************************************/

TEST(test_fake_heap, string_allocator)
{
  core::fake_heap_allocator_t<std::string> alloc(4 * sizeof(std::string));

  ASSERT_EQ(4, alloc.max_size());

  using traits_t = std::allocator_traits<decltype(alloc)>;

  traits_t::rebind_alloc<std::string> alloc_ = alloc;

  std::string* p = traits_t::allocate(alloc, 2); // место для двух строк

  ASSERT_NO_THROW(traits_t::construct(alloc, p, "foo"));
  ASSERT_NO_THROW(traits_t::construct(alloc, p + 1, "bar"));

  ASSERT_STREQ("foo", p[0].c_str());
  ASSERT_STREQ("bar", p[1].c_str());

  ASSERT_NO_THROW(traits_t::destroy(alloc, p + 1));
  ASSERT_NO_THROW(traits_t::destroy(alloc, p));
  ASSERT_NO_THROW(traits_t::deallocate(alloc, p, 2));

  EXPECT_THROW(p = traits_t::allocate(alloc, 3), std::bad_alloc); // больше двух не влезет
}

/******************************************************************************
  копирование allocator-а
  ...copies of an allocator must be able to free each others' memory...
*******************************************************************************/

TEST(test_fake_heap, allocator_copies)
{
  using alloc_type = core::fake_heap_allocator_t<char>;
  alloc_type::pointer p;
  alloc_type a1(1024);
  {
    alloc_type a2(a1);
    ASSERT_NO_THROW(p = std::allocator_traits<alloc_type>::allocate(a2, 14));
    ASSERT_NE(nullptr, p);
    strncpy(reinterpret_cast<char*>(p), "Hello, World!", 14);
  }
  ASSERT_STREQ("Hello, World!", reinterpret_cast<char*>(p));
  ASSERT_NO_THROW(std::allocator_traits<alloc_type>::deallocate(a1, p, 1));
}

/******************************************************************************
  проверка копирования состояний statefull allocator-а
*******************************************************************************/

struct A
{
  char hello[8]{ 'H','e','l','l','o',',',' ','\0' };
  std::string world{ "World!" };
};

struct B
{
  int key;
  B() : key(0) {}
  B(int key) : key(key) {}
  int cow{ 33 };
  double pi{ 3.14 };
};

TEST(test_fake_heap, statefull_allocator)
{
  bool heap_destroyed = false;

  auto fh_init = []() ->core::fake_heap_t* { std::cout << "INIT heap" << std::endl; return new core::fake_heap_t(1024, &debugger); };
  auto fh_deinit = [&heap_destroyed](core::fake_heap_t* ptr) { std::cout << "DEINIT heap" << std::endl; delete ptr; heap_destroyed = true; };
  std::weak_ptr<core::fake_heap_t> heap_observer;

  {
    ASSERT_EQ(0, heap_observer.use_count());
    ASSERT_FALSE(heap_destroyed);

    std::shared_ptr<core::fake_heap_t> fake_heap(fh_init(), fh_deinit);
    heap_observer = fake_heap;
    ASSERT_EQ(1, heap_observer.use_count());
    ASSERT_FALSE(heap_destroyed);

    using A_type = core::fake_heap_allocator_t<A>;
    using B_type = std::allocator_traits<A_type>::rebind_alloc<B>;
    A_type::pointer pa;
    B_type::pointer pb;

    A_type a1(fake_heap);
    ASSERT_EQ(2, heap_observer.use_count());
    {
      A_type a2(a1);
      ASSERT_EQ(3, heap_observer.use_count());
      ASSERT_NO_THROW(pa = std::allocator_traits<A_type>::allocate(a2, 1));
      ASSERT_NE(nullptr, pa);
      ASSERT_NO_THROW(std::allocator_traits<A_type>::construct(a2, pa));
      {
        B_type b1(a1);
        ASSERT_EQ(4, heap_observer.use_count());
        ASSERT_NO_THROW(pb = std::allocator_traits<B_type>::allocate(b1, 5));
        for (int i = 0; i < 5; ++i) { ASSERT_NO_THROW(std::allocator_traits<B_type>::construct(b1, pb + i, i)); }
        ASSERT_NO_THROW(B_type(a1).destroy(&pb[4]));
        ASSERT_EQ(4, heap_observer.use_count());
        {
          B_type b2(a2);
          ASSERT_EQ(5, heap_observer.use_count());
          ASSERT_NO_THROW(std::allocator_traits<B_type>::destroy(b2, &pb[3]));
          ASSERT_NO_THROW(std::allocator_traits<B_type>::destroy(b2, &pb[2]));
        }
        ASSERT_EQ(4, heap_observer.use_count());
      }
      B_type b3(a2);
      ASSERT_EQ(4, heap_observer.use_count());
      ASSERT_NO_THROW(std::allocator_traits<B_type>::destroy(b3, pb + 1));
    }
    ASSERT_EQ(2, heap_observer.use_count());

    ASSERT_STRCASEEQ("Hello, ", pa->hello);
    ASSERT_STRCASEEQ("World!", pa->world.c_str());
    ASSERT_EQ(33, pb->cow);
    ASSERT_EQ(3.14, pb->pi);

    ASSERT_NO_THROW(B_type(a1).destroy(pb));
    ASSERT_NO_THROW(B_type(fake_heap).deallocate(pb, 5));
    ASSERT_NO_THROW(std::allocator_traits<A_type>::destroy(a1, pa));
    ASSERT_NO_THROW(std::allocator_traits<A_type>::deallocate(a1, pa, 1));

    ASSERT_FALSE(heap_destroyed);
    ASSERT_EQ(2, heap_observer.use_count());
  }
  ASSERT_EQ(0, heap_observer.use_count());
  ASSERT_TRUE(heap_destroyed);
}

/******************************************************************************
  allocator, используемый в vector-е
*******************************************************************************/

TEST(test_fake_heap, vector_allocator)
{
  using alloc_t = core::fake_heap_allocator_t<int>;
  using vector_t = std::vector<int, alloc_t>;

  vector_t::allocator_type a(std::shared_ptr<core::fake_heap_t>(new core::fake_heap_t{ 100 }));
  vector_t v(a);

  ASSERT_EQ(25, v.get_allocator().max_size());

  ASSERT_NO_THROW(v.push_back(10));
  ASSERT_NO_THROW(v.push_back(20));
  ASSERT_NO_THROW(v.push_back(30));
  ASSERT_NO_THROW(v.push_back(40));
  ASSERT_NO_THROW(v.push_back(-1));

  ASSERT_EQ(5, v.size());

  ASSERT_EQ(10, v[0]);
  ASSERT_EQ(20, v[1]);
  ASSERT_EQ(30, v[2]);
  ASSERT_EQ(40, v[3]);
  ASSERT_EQ(-1, v[4]);
}

/******************************************************************************
  allocator, используемый в map-е
*******************************************************************************/

TEST(test_fake_heap, map_allocator)
{
  using alloc_t = core::fake_heap_allocator_t<std::pair<const int, std::string>>;
  using map_t = std::map<int, std::string, std::less<int>, alloc_t>;

  map_t::allocator_type a(std::shared_ptr<core::fake_heap_t>(new core::fake_heap_t{ 512, &debugger })); // 8+9*48
  map_t m(a);

  ASSERT_GT(m.get_allocator().max_size(), 15); // это всё равно неправильное число (размер pair<key,val>)

  ASSERT_NO_THROW(m[0] = "Клён ты мой опавший, клён заледенелый,");
  ASSERT_NO_THROW(m[1] = "Что стоишь, нагнувшись, под метелью белой?");
  ASSERT_NO_THROW(m[2] = "Или что увидел? Или что услышал?");
  ASSERT_NO_THROW(m[3] = "Словно за деревню погулять ты вышел");
  ASSERT_NO_THROW(m[4] = "И, как пьяный сторож, выйдя на дорогу,");
  ASSERT_NO_THROW(m[5] = "Утонул в сугробе, приморозил ногу.");
  ASSERT_NO_THROW(m[6] = "Ах, и сам я нынче чтой-то стал нестойкий,");
  ASSERT_NO_THROW(m[7] = "Не дойду до дома с дружеской попойки.");
  ASSERT_NO_THROW(m[8] = "Там вон встретил вербу, там сосну приметил,");
  EXPECT_THROW(   m[9] = "Распевал им песни под метель о лете.", std::bad_alloc);
  a.heap->enable_safe_mode();
  ASSERT_NO_THROW(m[9] = "Распевал им песни под метель о лете.");
  ASSERT_NO_THROW(m[10]= "Сам себе казался я таким же кленом,");
  ASSERT_NO_THROW(m[11]= "Только не опавшим, а вовсю зеленым.");
  ASSERT_NO_THROW(m[12]= "И, утратив скромность, одуревши в доску,");
  ASSERT_NO_THROW(m[13]= "Как жену чужую, обнимал березку.");

  ASSERT_EQ(14, m.size());

  ASSERT_STREQ("Клён ты мой опавший, клён заледенелый,", m[0].c_str());
  ASSERT_STREQ("Как жену чужую, обнимал березку.", m[13].c_str());
}

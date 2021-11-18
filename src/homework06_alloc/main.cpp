// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <map>
#include <vector>

#include <core/fake_heap_helpers.h>

#include "fake_heap_allocator.h"


class heap_logger_t : public core::fake_heap_debugger_t
{
public:
  virtual std::basic_ostream<char>& get() { return std::cout; }
} heap_logger;

struct A
{
  char hello[8]{'H','e','l','l','o',',',' ','\0'};
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

using map_t = std::map<const int, B, std::less<int>, fake_heap_allocator_t<std::pair<const int, B>>>;

int main()
{
  #if 0 // https://stackoverflow.com/a/22349253
  {
    using alloc_type = fake_heap_allocator_t<char /*, 1024*/>;
    alloc_type::pointer p;
    alloc_type a1;
    {
      alloc_type a2(a1);
      p = std::allocator_traits<alloc_type>::allocate(a2, 1);
    }
    std::allocator_traits<alloc_type>::deallocate(a1, p, 1);
  }
  #endif

  std::cout << std::endl;

  #if 0
  {
    auto fh_init = []() ->core::fake_heap_t* { std::cout << "!!!??? INIT ???!!!" << std::endl; return new core::fake_heap_t(1024, &heap_logger); };
    auto fh_deinit = [](core::fake_heap_t* ptr) { std::cout << "!!!??? DEINIT ???!!!" << std::endl; delete ptr; };
    std::weak_ptr<core::fake_heap_t> heap_observer;
    auto fh_observe = [&heap_observer](const char* msg = "") { std::cout << "!!!??? OBSERVE " << msg << " " << heap_observer.use_count() << "???!!!" << std::endl; };
    std::shared_ptr<core::fake_heap_t> fake_heap(fh_init(), fh_deinit);
    heap_observer = fake_heap;

    fh_observe("starting...");

    // Вот шаблон теста, который не должен приводить к проблемам при работе с памятью http://cpp.sh/8k4vt
    // В 20й строке замените std::allocator на свой тип - получите тестовую среду для домашнего задания по аллокаторам, в котором struct B - это элементы контейнера.
    using A_type = fake_heap_allocator_t<A>;    
    using B_type = std::allocator_traits<A_type>::rebind_alloc<B>;
    A_type a1(fake_heap);
    A_type::pointer pa;
    B_type::pointer pb;
    {
      A_type a2(a1);
      pa = std::allocator_traits<A_type>::allocate(a2, 1);
      std::allocator_traits<A_type>::construct(a2, pa);
      {
        B_type b1(a1);
        pb = std::allocator_traits<B_type>::allocate(b1, 5);
        for (int i = 0; i < 5; ++i) std::allocator_traits<B_type>::construct(b1, pb+i, i);
        B_type(a1).destroy(&pb[4]);
        {
          B_type b2(a2);
          std::allocator_traits<B_type>::destroy(b2, &pb[3]);
          std::allocator_traits<B_type>::destroy(b2, &pb[2]);
        }
      }
      B_type b3(a2);
      std::allocator_traits<B_type>::destroy(b3, pb+1);
    }
    std::cout << pa->hello << pa->world << " " << pb->cow << " " << pb->pi << std::endl;
    B_type(a1).destroy(pb);
    B_type(fake_heap).deallocate(pb, 5);
    std::allocator_traits<A_type>::destroy(a1, pa);
    std::allocator_traits<A_type>::deallocate(a1, pa, 1);

    fh_observe("finishing...");
  }
  #endif

  std::cout << std::endl;

  #if 0
  {
    // default allocator for ints
    fake_heap_allocator_t<int> alloc;// &heap_logger);

    // demonstrating the few directly usable members
    static_assert(std::is_same_v<int, decltype(alloc)::value_type>, "!is_same");
    int* p = alloc.allocate(1);
    alloc.deallocate(p, 1);

    // Even those can be used through traits though, so no need
    using traits_t = std::allocator_traits<decltype(alloc)>; // The matching trait
    p = traits_t::allocate(alloc, 1);
    traits_t::construct(alloc, p, 7); // construct the int
    std::cout << *p << std::endl;
    traits_t::deallocate(alloc, p, 1); // deallocate space for one int
  }
  #endif

  std::cout << std::endl;

  #if 0
  {
    // default allocator for strings
    fake_heap_allocator_t<std::string> alloc;
    // matching traits
    using traits_t = std::allocator_traits<decltype(alloc)>;

    // Rebinding the allocator using the trait for strings gets the same type
    traits_t::rebind_alloc<std::string> alloc_ = alloc;

    std::string* p = traits_t::allocate(alloc, 2); // space for 2 strings

    traits_t::construct(alloc, p, "foo");
    traits_t::construct(alloc, p + 1, "bar");

    std::cout << p[0] << ' ' << p[1] << '\n';

    traits_t::destroy(alloc, p + 1);
    traits_t::destroy(alloc, p);
    traits_t::deallocate(alloc, p, 2);
  }
  #endif

  std::cout << std::endl;

  #if 0
  std::vector<int, fake_heap_allocator_t<int>> v;
  v.push_back(10);
  v.push_back(10);
  v.push_back(10);
  v.push_back(10);
  v.push_back(10);
  #endif

  std::cout << std::endl;

  #if 0
  fake_heap_allocator_t<int> alloc;// &heap_logger);
  int* p = alloc.allocate(1);
  alloc.deallocate(p, 1);
  #endif

  std::cout << std::endl;

  #if 0
  constexpr const unsigned max_items = 10;
  // fake_heap_allocator_t<std::pair<const int, float>> allocator;// (&heap_logger); // max_items
  std::map<int, float, std::less<int>, fake_heap_allocator_t<std::pair<const int, float>>> m; // (allocator);
  //m.get_allocator().attach_debugger(&heap_logger);

  for (int i = 0; i < 3; i++)
  {
    m[i] = static_cast<float>(i);
  }
  #endif

  std::cout << std::endl;

  #if 0
  {
    std::cout << "map_t::node_type = " << sizeof(map_t::node_type) << std::endl;
    std::cout << "map_t::value_type = " << sizeof(map_t::value_type) << std::endl;

    std::weak_ptr<core::fake_heap_t> heap_observer;

    auto fh_init = []() ->core::fake_heap_t* { std::cout << "!!!??? INIT ???!!!" << std::endl; return new core::fake_heap_t(1024, &heap_logger); };
    auto fh_deinit = [](core::fake_heap_t* ptr) { std::cout << "!!!??? DEINIT ???!!!" << std::endl; delete ptr; };
    auto fh_observe = [&heap_observer](const char* msg = "") { std::cout << "!!!??? OBSERVE " << msg << " " << heap_observer.use_count() << "???!!!" << std::endl; };

    fh_observe("begin");
    {
      std::shared_ptr<core::fake_heap_t> fake_heap(fh_init(), fh_deinit);
      heap_observer = fake_heap;

      fh_observe("connect");

      map_t::allocator_type::pointer p;
      map_t::allocator_type a1(fake_heap);
      fh_observe("allocator a1 begin");
      {
        map_t::allocator_type a2(a1);
        fh_observe("allocator a2 begin");
        p = std::allocator_traits<map_t::allocator_type>::allocate(a2, 1);
      }
      fh_observe("allocator a2 end");
      std::allocator_traits<map_t::allocator_type>::deallocate(a1, p, 1);

      fh_observe("disconnect");
    }
    fh_observe("stop");
  }
  #endif

  std::cout << std::endl;

  #if 1
  {
    auto fh_init = []() ->core::fake_heap_t* { std::cout << "!!!??? INIT ???!!!" << std::endl; return new core::fake_heap_t(1024, &heap_logger); };
    auto fh_deinit = [](core::fake_heap_t* ptr) { std::cout << "!!!??? DEINIT ???!!!" << std::endl; delete ptr; };
    //std::shared_ptr<core::fake_heap_t> fake_heap(fh_init(), fh_deinit);
    //map_t::allocator_type alloc(fake_heap);

    //map_t m(alloc);

    map_t m(map_t::allocator_type{ std::shared_ptr<core::fake_heap_t>(fh_init(), fh_deinit) });

    std::cout << std::endl << "copy assign" << std::endl << std::endl;
    m[1] = B{1};
    m.get_allocator().heap.reset();
    std::cout << std::endl << "insert" << std::endl << std::endl;
    for (int i = 0; i < 2; i++)
      m.insert(map_t::value_type(i, B{i}));
    std::cout << std::endl << "assign" << std::endl << std::endl;
    map_t m2(m);
    std::cout << std::endl << "delete" << std::endl << std::endl;
  }
  #endif

  return EXIT_SUCCESS;
}

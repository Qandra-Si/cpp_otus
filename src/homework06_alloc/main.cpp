// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <cstdlib>
#include <map>

#include <cpp_otus_config.h> // __PRETTY_FUNCTION__


template<typename T>
struct fake_heap_alloc_t
{
  using value_type = T;

  template<typename U>
  struct rebind {
    using other = fake_heap_alloc_t<U>;
  };

  fake_heap_alloc_t()
  {
    std::cout << "ctr" << std::endl;
  }
  ~fake_heap_alloc_t() = default;
  fake_heap_alloc_t(const fake_heap_alloc_t<T>&);

  template<typename U>
  fake_heap_alloc_t(const fake_heap_alloc_t<U>&) { }

  T *allocate(std::size_t n)
  {
    auto p = std::malloc(n * sizeof(T));
    if (!p)
      throw std::bad_alloc();
    std::cout << __PRETTY_FUNCTION__ << " [n = " << n << ", p = " << p << ", szT*n = " << n*sizeof(T) << "]" << std::endl;
    return reinterpret_cast<T*>(p);
  }

  void deallocate(T * p, std::size_t n)
  {
    std::cout << __PRETTY_FUNCTION__ << " [n = " << n << ", p = " << p << "]" << std::endl;
    std::free(p);
  }

  template<typename U, typename ...Args>
  void construct(U * p, Args &&...args)
  {
    std::cout << __PRETTY_FUNCTION__ << " [p = " << p << ", szT = " << sizeof(U); /* << ", ";
    int a[sizeof...(args)] = {(std::cout << args, 0)...};*/
    std::cout << "]" << std::endl;
    new(p) U(std::forward<Args>(args)...);
  }

  void destroy(T * p)
  {
    std::cout << __PRETTY_FUNCTION__ << " [p = " << p << "]" << std::endl;
    p->~T();
  }
private:
  
};

int main()
{
  std::map<int, float, std::less<int>, fake_heap_alloc_t<std::pair<const int, float>>> m;
  for (int i = 0; i < 3; i++)
  {
    m[i] = static_cast<float>(i);
  }
  return EXIT_SUCCESS;
}

#include <iostream>
#include <vector>

#include <cpp_otus_config.h> // __PRETTY_FUNCTION__


void example1();
void example2();

int main()
{
  example1();
  example2();
  return 0;
}

template<typename T>
struct my_allocator_t
{
  // блок деклараций, который от нас ожидает allocator
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;

  template<typename U>
  struct rebind {
    using other = my_allocator_t<U>;
  };

  my_allocator_t() = default;
  ~my_allocator_t() = default;
  my_allocator_t(const my_allocator_t<T>&);

  template<typename U>
  my_allocator_t(const my_allocator_t<U>&) { }

  T *allocate(std::size_t n)
  {
    auto p = std::malloc(n * sizeof(T));
    if (!p)
      throw std::bad_alloc();
    std::cout << __PRETTY_FUNCTION__ << " [n = " << n << ", p = " << p << "]" << std::endl;
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
    std::cout << __PRETTY_FUNCTION__ << " [p = " << p; /* << ", ";
    int a[sizeof...(args)] = {(std::cout << args, 0)...};*/
    std::cout << "]" << std::endl;
    new(p) U(std::forward<Args>(args)...);
  }

  void destroy(T * p)
  {
    std::cout << __PRETTY_FUNCTION__ << " [p = " << p << "]" << std::endl;
    p->~T();
  }
};

void example1()
{
  std::cout << "example1:" << std::endl;

  std::vector<int, my_allocator_t<int>> v;
  for (int i = 0; i < 3; i++)
  {
    v.emplace_back();
  }
  std::cout << "* all elements allocated, ... exiting" << std::endl;
}

void example2()
{
  std::cout << "\nexample2:" << std::endl;

  std::vector<int, my_allocator_t<int>> v;
  v.reserve(3);
  for (int i = 0; i < 3; i++)
  {
    v.emplace_back();
  }
  std::cout << "* all elements allocated, ... exiting" << std::endl;
}


#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <unordered_map>

#include <cpp_otus_config.h> // __PRETTY_FUNCTION__


void example1();
void example2();
void example3();
void example4();

int main()
{
  example1();
  example2();
  example3();
  example4();
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
};

void example1()
{
  std::cout << "example1 (vector resizing):" << std::endl;

  std::vector<int, my_allocator_t<int>> v;
  for (int i = 0; i < 3; i++)
  {
    v.emplace_back();
  }
  std::cout << "* all elements allocated, ... exiting" << std::endl;
}

void example2()
{
  std::cout << "\nexample2 (reserved vector):" << std::endl;

  std::vector<int, my_allocator_t<int>> v;
  v.reserve(3);
  for (int i = 0; i < 3; i++)
  {
    v.emplace_back();
  }
  std::cout << "* all elements allocated, ... exiting" << std::endl;
}

void example3()
{
  std::cout << "\nexample3 (map):" << std::endl;

  std::map<int, float, std::less<int>, my_allocator_t<std::pair<const int, float>>> m;
  for (int i = 0; i < 3; i++)
  {
    m[i] = static_cast<float>(i);
  }

  std::cout << "* all elements allocated, ... clearing" << std::endl;

  m.clear();

  std::map<int, float> m2;
  for (int i = 10; i < 100; i += 10)
  {
    m2[i] = static_cast<float>(i*10);
  }

  std::cout << "* all elements cleared, ... inserting" << std::endl;

  m.insert(m2.begin(), m2.end()); // данные всё равно добавляются последовательно

  std::cout << "* all elements allocated, ... exiting" << std::endl;
}

void example4()
{
  std::cout << "\nexample4 (unordered map):" << std::endl;

  std::unordered_map<int, float, std::hash<int>, std::equal_to<int>, my_allocator_t<std::pair<const int, float>>> m;
  m.reserve(5);
  for (int i = 0; i < 5; i++)
  {
    m.insert(std::pair<int, float>(i, static_cast<float>(i)));
  }
  std::cout <<  "* all elements allocated, ... exiting" << std::endl;
}


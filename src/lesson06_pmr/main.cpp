#include <iostream>
#include <vector>
#include <memory_resource>

#include <cpp_otus_config.h> // __PRETTY_FUNCTION__


void example1();

int main()
{
  example1();
  return 0;
}

template<typename T>
struct my_allocator_t : public std::pmr::memory_resource
{
  using value_type = T;

  my_allocator_t() = default;

  virtual void * do_allocate(std::size_t n, size_t alignment) override
  {
    auto p = std::malloc(n * sizeof(T));
    if (!p)
      throw std::bad_alloc();
    std::cout << __PRETTY_FUNCTION__ << " [n = " << n << "(" << alignment << ")" << ", p = " << p << ", szT*n = " << n*sizeof(T) << "]" << std::endl;
    return p;
  }

  virtual void do_deallocate(void * p, std::size_t n, size_t alignment) override
  {
    std::cout << __PRETTY_FUNCTION__ << " [n = " << n << "(" << alignment << ")" << ", p = " << p << "]" << std::endl;
    std::free(p);
  }

  bool do_is_equal(const std::pmr::memory_resource & ) const noexcept override
  {
    return false;
  }
};

void foo(const std::pmr::vector<int> & ) { }

void example1()
{
  std::cout << "example1:" << std::endl;

  std::cout << "\n* stage 1:" << std::endl;

  std::pmr::vector<int> v1{0, 1, 2, 3, 4, 5};
  foo(v1); // Ok

  std::cout << "\n* stage 2:" << std::endl;

  my_allocator_t<int> alloc2;
  std::pmr::vector<int> v2(&alloc2);
  v2.push_back(100);
  v2.push_back(-100);
  foo(v2); // Ok again!

  std::cout << "\n* stage 3:" << std::endl;

  std::pmr::vector<int> v3{6, 7, 8, 9};
  v1 = v3; // Ok again!

  std::cout << "\n* stage 4:" << std::endl;
  v1 = v2; // Ok again!

  std::cout <<  "* all elements allocated, ... exiting" << std::endl;
}

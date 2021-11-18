// -*- mode: c++; coding: utf-8 -*-
#pragma once

#include <memory>

#include <cpp_otus_config.h>
#include <core/fake_heap.h>


#if 0
template <class T>
struct custom_allocator
{
  using value_type = T;
  custom_allocator() noexcept {}
  template <class U> custom_allocator(const custom_allocator<U>&) noexcept {}
  T* allocate(std::size_t n, const void* hint = 0)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    return (T*)std::malloc(n * sizeof(T));
  }
  void deallocate(T* p, std::size_t n)
  {
    std::free(p);
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }

  // The rebind member allows a container to construct an allocator for some arbitrary type out of
  // the allocator type provided as a template parameter.
  template<typename U>
  struct rebind {
    using other = custom_allocator<U>;
  };
};

template <class T, class U>
constexpr bool operator== (const custom_allocator<T>&, const custom_allocator<U>&) noexcept
{
  return false;
}

template <class T, class U>
constexpr bool operator!= (const custom_allocator<T>&, const custom_allocator<U>&) noexcept
{
  return true;
}
#endif


/*! \brief Аллокатор на базе фейковой кучи \ref core::fake_heap_t
 */
template<typename T = void>
class fake_heap_allocator_t
{
public:
  // блок деклараций, который от нас ожидает allocator
  using size_type = std::size_t;
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;

  // блок деклараций, который позволяет allocator-у иметь состояние
//  using propagate_on_container_copy_assignment = std::false_type;
//  using propagate_on_container_move_assignment = std::false_type;
//  using propagate_on_container_swap = std::false_type;
//  using is_always_equal = std::false_type;

  // determines if memory from another allocator can be deallocated from this one
//  bool operator==(const fake_heap_allocator_t& other) const { return this == &other; }
//  bool operator!=(const fake_heap_allocator_t& other) const { return !(*this == other); }

  // The rebind member allows a container to construct an allocator for some arbitrary type out of
  // the allocator type provided as a template parameter.
  template<typename U>
  struct rebind {
    using other = fake_heap_allocator_t<U>;
  };

  // Constructors
  fake_heap_allocator_t(std::shared_ptr<core::fake_heap_t> heap) : heap(heap)
  {
    std::cout << " * (0) "  << __PRETTY_FUNCTION__ << " --- " << sizeof(T) << " === " << typeid(T).name() << std::endl;
  }
  fake_heap_allocator_t(fake_heap_allocator_t& other) : heap(other.heap)
  {
    std::cout << " * (1) " << __PRETTY_FUNCTION__ << " --- " << sizeof(T) << " === " << typeid(T).name() << std::endl;
  }
  template<typename U>
  fake_heap_allocator_t(const fake_heap_allocator_t<U>& other) : heap(other.heap)
  {
    std::cout << " * (2) " << __PRETTY_FUNCTION__ << " --- " << sizeof(T) << " === " << typeid(T).name() << " --- " << sizeof(U) << " === " << typeid(U).name() << std::endl;
    //if (!std::is_same<T, U>::value)
    //  rebind_alloc = new std::allocator<T>();
  }
  // Destructor
  ~fake_heap_allocator_t()
  {
    std::cout << " * (3) " << __PRETTY_FUNCTION__ << " --- " << sizeof(T) << " === " << typeid(T).name() << std::endl;
  }

  // Allocate storage for n values of T
  T *allocate(size_type n, const void* = 0)
  {
    std::cout << __PRETTY_FUNCTION__ << " [n = " << n << ", szT*n = " << n * sizeof(T)
              //<< " === " << std::endl << std::endl << typeid(T).name() << std::endl
              //<< "(!!!" << typeid(map_t::value_type).name() << "!!!" << std::is_same<T, map_t::value_type>::value << "!!!)"
              << "]" << std::endl;
    auto p = heap->malloc(n * sizeof(T));
    std::cout << __PRETTY_FUNCTION__ << " [n = " << n << ", szT*n = " << n * sizeof(T) << ", p = " << (const void*)p << "]" << std::endl;
    if (!p)
      throw std::bad_alloc();
    return reinterpret_cast<T*>(p);
  }

  // Deallocate storage obtained by a call to allocate
  void deallocate(T * p, size_type n)
  {
    std::cout << __PRETTY_FUNCTION__ << " [n = " << n << ", p = " << (const void*)p << "]" << std::endl;
    heap->free(p);
  }
  
  // Construct an object of type T at the location of ptr
  void construct(pointer ptr, const_reference val)
  {
    ::new (reinterpret_cast<void*>(ptr)) T(val);
  }

  // Construct an object of type T at the location of ptr, using the value of U in the call to the
  // constructor for T
  template <class U, typename ...Args>
  void construct(U* ptr, Args&&... args)
  {
    std::cout << __PRETTY_FUNCTION__ << " [p = " << (const void*)ptr << ", szT = " << sizeof(T); /* << ", ";
    int a[sizeof...(args)] = {(std::cout << args, 0)...};*/
    std::cout << "]" << std::endl;
    ::new (reinterpret_cast<void*>(ptr)) U(std::forward<Args>(args)...);
  }

  // Call the destructor on the value pointed to by ptr
  void destroy(T * ptr)
  {
    std::cout << __PRETTY_FUNCTION__ << " [p = " << (const void*)ptr << "]" << std::endl;
    ptr->~T();
  }

  // Returns the maximum theoretically possible value of n, for which the call allocate(n, 0) could succeed.
  size_type max_size() const
  {
    // возвращаем кол-во элементов, которые могли бы оказаться в выделенном
    // блоке памяти, но только это чушь... т.к. из std::allocator_traits<T>::rebind_alloc<U>
    // в выделенный блок памяти также попадают U-элементы и уменьшают кол-во
    // T-элементов, которые могут попасть в нашу фейковую кучу ...с другой
    // стороны этот метод по дефолту возвращает size_t(-1) / sizeof(T)...
    return heap->get_heap_size() / sizeof(T);
  }

  /*! \brief Устанавливает флаг использования кучи, если в фейковой не хватило памяти */
  inline void enable_safe_mode() { heap.enable_safe_mode(); }
  /*! \brief Сбрасывает флаг использования кучи, если в фейковой не хватило памяти */
  inline void disable_safe_mode() { heap.disable_safe_mode(); }

  /*! \brief Подключает отладчик кучи, для вывода сообщений в поток */
  inline void attach_debugger(core::fake_heap_debugger_t* debugger) { heap.attach_debugger(debugger); }
  /*! \brief Отключает отладчик кучи, для вывода сообщений в поток */
  inline void detach_debugger() { heap.detach_debugger(); }

  std::shared_ptr<core::fake_heap_t> heap;
};

/*
// Return true if allocators b and a can be safely interchanged. "Safely interchanged" means that b could be
// used to deallocate storage obtained through a and vice versa.
template <class T1, class T2>
bool operator==(const fake_heap_allocator_t<T1>& a, const fake_heap_allocator_t<T2>& b) noexcept
{
    return false;
}

// Return false if allocators b and a can be safely interchanged. "Safely interchanged" means that b could be
// used to deallocate storage obtained through a and vice versa.
template <class T1, class T2> bool operator != (const fake_heap_allocator_t<T1>& a, const fake_heap_allocator_t<T2>& b)
{
  return true;
}
*/

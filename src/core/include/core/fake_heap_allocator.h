// -*- mode: c++; coding: utf-8 -*-
#pragma once

#include <memory>

#include <core/fake_heap.h>


namespace core {

/*! \brief Аллокатор на базе фейковой кучи \ref fake_heap_t
*/
template<typename T>
struct fake_heap_allocator_t
{
  // данные, которые хранятся в allocator-е, тем самым он становится не
  // zerosize-class, а statefull allocator (но непосредственно сами данные
  // находятся "далеко", с доступом по shared-указателю)
  std::shared_ptr<fake_heap_t> heap;

  // блок деклараций, который от нас ожидает allocator
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;

  // блок деклараций, которые от нас ожидает allocator_traits
  using size_type = std::size_t;
  //using propagate_on_container_copy_assignment = std::false_type;
  //using propagate_on_container_move_assignment = std::false_type;
  //using propagate_on_container_swap = std::false_type;
  //using is_always_equal = std::false_type;

  // determines if memory from another allocator can be deallocated from this one
  bool operator==(const fake_heap_allocator_t& other) const { return this == &other; }
  bool operator!=(const fake_heap_allocator_t& other) const { return !(*this == other); }

  // The rebind member allows a container to construct an allocator for some arbitrary type out of
  // the allocator type provided as a template parameter.
  template<typename U>
  struct rebind {
    using other = fake_heap_allocator_t<U>;
  };

  // Constructors
  fake_heap_allocator_t(std::size_t heap_size) : heap(std::shared_ptr<fake_heap_t>(new fake_heap_t(heap_size))) { }
  fake_heap_allocator_t(std::shared_ptr<fake_heap_t> heap) : heap(heap) { }
  fake_heap_allocator_t(fake_heap_allocator_t& other) : heap(other.heap) { }
  template<typename U>
  fake_heap_allocator_t(const fake_heap_allocator_t<U>& other) : heap(other.heap) { }
  // Destructor
  ~fake_heap_allocator_t() = default;

  // Allocate storage for n values of T
  T *allocate(size_type n, const void* /*[[maybe_unused]] y*/ = 0)
  {
    // C++ [allocator.requirements.general] #5 Note C
    // If n == 0, the return value is unspecified.
    if (n == 0) return nullptr;
    // Memory is allocated for an array of n T and such an object is created but
    // array elements are not constructed.
    auto p = heap->malloc(n * sizeof(T));
    if (!p)
      throw std::bad_alloc();
    return reinterpret_cast<T*>(p);
  }

  // Deallocate storage obtained by a call to allocate
  void deallocate(T * p, size_type /*[[maybe_unused]] n*/)
  {
    // C++ [allocator.requirements.general] #1
    // p is a pointer value obtained from allocate. n equals the value passed
    // as the first argument to the invocation of allocate which returned p
    heap->free(p);
  }

  // Construct an object of type T at the location of ptr
  void construct(pointer ptr, const_reference val)
  {
    ::new (reinterpret_cast<void*>(ptr)) T(val);
  }

  // Construct an object of type T at the location of ptr, using the value of
  // U in the call to the constructor for T
  template <class U, typename ...Args>
  void construct(U* ptr, Args&&... args)
  {
    ::new (reinterpret_cast<void*>(ptr)) U(std::forward<Args>(args)...);
  }

  // Call the destructor on the value pointed to by ptr
  void destroy(T * ptr)
  {
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
    if (heap->is_safe_mode())
      return size_t(-1) / sizeof(T);
    else
      return heap->get_heap_size() / sizeof(T);
  }
};

// Return true if allocators b and a can be safely interchanged. "Safely interchanged" means that b could be
// used to deallocate storage obtained through a and vice versa.
template <class T1, class T2>
bool operator==(const fake_heap_allocator_t<T1>& a, const fake_heap_allocator_t<T2>& b) noexcept
{
  if (!std::is_same<T1,T2>::value) return false;
  return a == b;
}

// Return false if allocators b and a can be safely interchanged. "Safely interchanged" means that b could be
// used to deallocate storage obtained through a and vice versa.
template <class T1, class T2>
bool operator != (const fake_heap_allocator_t<T1>& a, const fake_heap_allocator_t<T2>& b)
{
  if (std::is_same<T1, T2>::value) return true;
  return a != b;
}

}

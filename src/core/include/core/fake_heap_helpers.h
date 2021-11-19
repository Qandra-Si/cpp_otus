// -*- mode: c++; coding: utf-8 -*-
#pragma once

#include <iostream>


namespace core {

/*! \brief Отладчик фейковой кучи \ref fake_heap_t
 *
 * Абстракный класс, в котором надо переопределить метод \ref get, и с помощью
 * него направить отладочные сообщений в консоль. Можно использовать в качестве
 * моста для проведения тестирований - проверять поведение \ref fake_heap_t.
 *
 * В этом файле этот класс находится в первую очередь потому, что вся эта
 * отладка сильно замусоривает основной алгоритм фейковой кучи, до нечитаемого
 * вида.
 */
class fake_heap_debugger_t
{
public:
  virtual std::basic_ostream<char> & get() = 0;

  virtual void reset(const void* beg, const void* end)
  {
    get() << std::endl << "=== > fake_heap.reset from fake:" << beg << " to fake:" << end << std::endl;
  }
  virtual void fake_calloc(const void* beg, const void* end, unsigned size)
  {
    get() << "=== > fake_heap.calloc from fake:" << beg << " to fake:" << end << ", size " << size << std::endl;
  }
  virtual void fake_malloc(const void* beg, const void* end, unsigned size)
  {
    get() << "=== > fake_heap.malloc from fake:" << beg << " to fake:" << end << ", size " << size << std::endl;
  }
  virtual void fake_free(const void* ptr)
  {
    get() << "=== > fake_mem.free fake:" << ptr << std::endl;
  }

  virtual void real_calloc(const void* ptr, unsigned size)
  {
    get() << "=== > stdlib.calloc heap:" << ptr << " size " << size << std::endl;
  }
  virtual void real_malloc(const void* ptr, unsigned size)
  {
    get() << "=== > stdlib.malloc heap:" << ptr << " size " << size << std::endl;
  }
  virtual void real_realloc(const void* ptr, const void * reptr, unsigned size)
  {
    get() << "=== > stdlib.realloc from heap:" << ptr << " to heap:" << reptr << " size " << size << std::endl;
  }
  virtual void real_free(const void* ptr)
  {
    get() << "=== > stdlib.free heap:" << ptr << std::endl;
  }

  virtual void fake_recalloc(const void* beg, const void* end, unsigned hdr_size, unsigned chunk_size, unsigned size)
  {
    get()
      << "=== > fake_heap.calloc from fake:" << beg << " to fake:" << end
      << ", size " << hdr_size << "+" << chunk_size << " (adopted from " << size
      << ")" << std::endl;
  }
  virtual void fake_remalloc(const void* beg, const void* end, unsigned hdr_size, unsigned chunk_size, unsigned size)
  {
    get()
      << "=== > fake_heap.malloc from fake:" << beg << " to fake:" << end
      << ", size " << hdr_size << "+" << chunk_size << " (adopted from " << size
      << ")" << std::endl;
  }
  virtual void fake_realloc(const void* ptr, const void* reptr, unsigned new_size, unsigned chunk_size,  bool stayed_in_chunk)
  {
    get()
      << "=== > fake_mem.realloc from fake:" << ptr << " to fake:" << reptr
      << ", new size " << new_size << (stayed_in_chunk?" (stayed in chunk ":" (readopted from ")
      << chunk_size << ")" << std::endl;
  }
  virtual void fake_realloc_move2fake(const void* ptr, const void* reptr, unsigned new_size, unsigned chunk_size, unsigned old_size)
  {
    get()
      << "=== === > fake_mem.realloc moved from fake:" << ptr << " to fake:"
      << reptr << ", new size " << new_size << " (adopted to " << (unsigned)chunk_size
      << ", readopted from " << old_size << ")" << std::endl;
  }
  virtual void fake_realloc_move2heap(const void* ptr, const void* reptr, unsigned new_size, unsigned old_size)
  {
    get()
      << "=== === > fake_mem.realloc moved from fake:" << ptr << " to heap:"
      << reptr << ", new size " << new_size << " (readopted from " << old_size << ")"
      << std::endl;
  }
};

}

// -*- mode: c++; coding: utf-8 -*-

#include <cstdlib> // malloc, calloc, free, realloc
#include <cstring> // memset
#include <cassert> // assert
#include <new> // std::bad_alloc

#include <core/fake_heap.h>
#include <core/fake_heap_helpers.h>



namespace core {

//------------------------------------------------------------------------------
// fake_heap_t
//------------------------------------------------------------------------------
fake_heap_t::fake_heap_t(std::size_t heap_size, fake_heap_debugger_t* debugger)
 : m_mem((uint8_t*)::malloc(heap_size)),
   m_end(m_mem + heap_size),
   m_heap_size(heap_size),
   m_safe_mode(false)
{
  attach_debugger(debugger);
  reset();

  // куча должна позволить разместить хотя-бы один октет
  assert(is_enough(1));
}

fake_heap_t::~fake_heap_t()
{
  ::free(m_mem);
}

void fake_heap_t::reset()
{
  m_cursor = m_mem;
  if (m_debugger)
  {
    m_debugger->get()
      << std::endl << "=== > fake_heap.reset from fake:" << (const void*)m_cursor
      << " to fake:" << (const void*)m_end << std::endl;
  }
}

void * fake_heap_t::calloc(std::size_t nmemb, std::size_t size)
{
  const unsigned chunk_size = size * nmemb;
  if (is_enough(size))
  {
    void * beg = m_cursor;
    uint8_t * end = m_cursor + chunk_size;
    memset(beg, 0, chunk_size);
    if (m_debugger)
    {
      m_debugger->get()
        << "=== > fake_heap.calloc from fake:" << (const void*)beg << " to fake:"
        << (const void*)end << ", size " << chunk_size << std::endl;
    }
    m_cursor = end;
    return beg;
  }
  else if (m_safe_mode)
  {
    return cstdlib_calloc(nmemb, size);
  }
  else
  {
    // памяти не хватило в fake-буфере, при этом safe_mode отключен
    throw std::bad_alloc();
  }
}

void * fake_heap_t::malloc(std::size_t size)
{
  if (is_enough(size))
  {
    void * beg = m_cursor;
    uint8_t * end = m_cursor + size;
    if (m_debugger)
    {
      m_debugger->get()
        << "=== > fake_mem.malloc from fake:" << (const void*)beg << " to fake:"
        << (const void*)end << ", size " << size << std::endl;
    }
    m_cursor = end;
    return beg;
  }
  else if (m_safe_mode)
  {
    return cstdlib_malloc(size);
  }
  else
  {
    // памяти не хватило в fake-буфере, при этом safe_mode отключен
    throw std::bad_alloc();
  }
}

void* fake_heap_t::cstdlib_calloc(std::size_t nmemb, std::size_t size)
{
  void* ptr = ::calloc(nmemb, size);
  if (m_debugger)
  {
    m_debugger->get()
      << "=== > stdlib.calloc heap:" << (const void*)ptr << " size "
      << (unsigned)(nmemb * size) << std::endl;
  }
  return ptr;
}

void* fake_heap_t::cstdlib_malloc(size_t size)
{
  void * ptr = ::malloc(size);
  if (m_debugger)
  {
    m_debugger->get()
      << "=== > stdlib.malloc heap:" << (const void*)ptr << " size "
      << size << std::endl;
  }
  return ptr;
}

void fake_heap_t::free(void * ptr)
{
  if (ptr == nullptr) return;
  if (is_own(ptr))
  {
    // ничего не делаем, всё в порядке
    if (m_debugger)
    {
      m_debugger->get()
        << "=== > fake_mem.free fake:" << (const void*)ptr << std::endl;
    }
  }
  else
  {
    if (m_debugger)
    {
      m_debugger->get()
        << "=== > stdlib.free heap:" << (const void*)ptr << std::endl;
    }
    // память блока размещена в куче (не хватало памяти)
    ::free(ptr);
  }
}

//------------------------------------------------------------------------------
// fake_realloc_heap_t
//------------------------------------------------------------------------------
fake_realloc_heap_t::fake_realloc_heap_t(std::size_t heap_size, std::size_t chunk_size, fake_heap_debugger_t* debugger)
 : fake_heap_t(heap_size, debugger),
   m_heap_chunk_size(chunk_size)
{
  // куча не м.б. меньше пользовательского чанка
  assert(is_enough(sizeof(realloc_size_t) + mem_chunk_adoptation(1)));
}

unsigned fake_realloc_heap_t::mem_chunk_adoptation(std::size_t size) const
{
  const unsigned rest = size % m_heap_chunk_size;
  if (rest)
    return size + m_heap_chunk_size - rest;
  else
    return size;
}

void* fake_realloc_heap_t::calloc(size_t nmemb, size_t size)
{
  const unsigned membs_size = size * nmemb;
  const unsigned chunk_size = mem_chunk_adoptation(membs_size);
  if (is_enough(sizeof(realloc_size_t) + chunk_size))
  {
    uint8_t * cursor = get_cursor(), * tail = cursor + sizeof(realloc_size_t) + chunk_size;
    realloc_size_t * beg = reinterpret_cast<realloc_size_t*>(cursor);
    *beg++ = static_cast<realloc_size_t>(chunk_size);
    memset(beg, 0, membs_size);
    if (get_debugger())
    {
      get_debugger()->get()
        << "=== > fake_heap.calloc from fake:" << (const void*)beg << " to fake:"
        << (const void*)tail << ", size " << sizeof(realloc_size_t) << "+"
        << chunk_size << " (adopted from " << membs_size << ")" << std::endl;
    }
    move_cursor(tail);
    return beg;
  }
  else if (is_safe_mode())
  {
    return cstdlib_calloc(nmemb, size);
  }
  else
  {
    // памяти не хватило в fake-буфере, при этом safe_mode отключен
    throw std::bad_alloc();
  }
}

void * fake_realloc_heap_t::malloc(size_t size)
{
  const unsigned chunk_size = mem_chunk_adoptation(size);
  if (is_enough(sizeof(realloc_size_t) + chunk_size))
  {
    uint8_t * cursor = get_cursor(), * tail = cursor + sizeof(realloc_size_t) + chunk_size;
    realloc_size_t * beg = reinterpret_cast<realloc_size_t*>(cursor);
    *beg++ = static_cast<realloc_size_t>(chunk_size);
    if (get_debugger())
    {
      get_debugger()->get()
        << "=== > fake_mem.malloc from fake:" << (const void*)beg << " to fake:"
        << (const void*)tail << ", size " << sizeof(realloc_size_t) << "+"
        << chunk_size << " (adopted from " << (unsigned)size << ")" << std::endl;
    }
    move_cursor(tail);
    return beg;
  }
  else if (is_safe_mode())
  {
    return cstdlib_malloc(size);
  }
  else
  {
    // памяти не хватило в fake-буфере, при этом safe_mode отключен
    throw std::bad_alloc();
  }
}

void * fake_realloc_heap_t::realloc(void * ptr, size_t size)
{
  if (ptr == nullptr)
    return malloc(size);
  else if (size == 0)
  {
    free(ptr);
    return nullptr;
  }
  else if (is_own(ptr))
  {
    uint8_t * p8 = reinterpret_cast<uint8_t*>(ptr);
    realloc_size_t * chunk_size = reinterpret_cast<realloc_size_t*>(ptr) - 1;
    // новый фрагмент находится внутри выделенного ранее chunk-а
    if (size <= *chunk_size)
    {
      if (get_debugger())
      {
        get_debugger()->get()
          << "=== > fake_mem.realloc from fake:" << (const void*)ptr
          << " to fake:" << (const void*)(p8 + *chunk_size) << ", size "
          << (unsigned)size << " (stayed in chunk " << *chunk_size << ")"
          << std::endl;
      }
      return ptr;
    }
    // текущий chunk размещался последним, достаточно просто увеличить его
    // размер подвинув курсор, но только, если достаточно памяти
    else if ((get_cursor() == (p8 + *chunk_size)) && is_enough(m_heap_chunk_size))
    {
      if (get_debugger())
      {
        get_debugger()->get()
          << "=== > fake_mem.realloc from fake:" << (const void*)p8 << " to fake:"
          << (const void*)(p8 + *chunk_size + m_heap_chunk_size) << ", size "
          << *chunk_size + m_heap_chunk_size << " (readopted from " << *chunk_size
          << ")" << std::endl;
      }
      *chunk_size += m_heap_chunk_size;
      move_cursor(get_cursor() + m_heap_chunk_size);
      return ptr;
    }
    // освобождаем (забываем о существовании) предыдущего чанка и создаём новый
    else
    {
      void * reptr = malloc(size);
      // что, даже в куче места не хватило?
      if (ptr == nullptr) throw std::bad_alloc();
      // здесь возможно лишнее копирование, т.к. chunk_size м.б. больше чем size
      // в том случае, если с этим указателем делался realloc на меньший размер
      memcpy(reptr, ptr, *chunk_size);
      if (get_debugger())
      {
        if (is_own(reptr))
          get_debugger()->get()
            << "=== === > fake_mem.realloc moved from fake:" << (const void*)ptr
            << " to fake:" << (const void*)reptr << ", new size "
            << *(((realloc_size_t*)reptr) - 1) << " (adopted from " << (unsigned)size
            << ", readopted from " << *chunk_size << ")" << std::endl;
        else
          get_debugger()->get()
            << "=== === > fake_mem.realloc moved from fake:" << (const void*)ptr
            << " to heap:" << (const void*)reptr << ", new size "
            << (unsigned)size << " (readopted from " << *chunk_size << ")"
            << std::endl;
      }
      return reptr;
    }
  }
  else if (is_safe_mode())
  {
    // память блока размещена в куче (не хватало памяти)
    void * reptr = ::realloc(ptr, size);
    if (get_debugger())
    {
      get_debugger()->get()
        << "=== > stdlib.realloc from heap:" << (const void*)ptr << " to heap:"
        << (const void*)reptr << " size " << size << std::endl;
    }
    return reptr;
  }
  else
  {
    // памяти не хватило в fake-буфере, при этом safe_mode отключен
    throw std::bad_alloc();
  }
}

}

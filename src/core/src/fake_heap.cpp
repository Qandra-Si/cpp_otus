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
 : m_mem(reinterpret_cast<uint8_t*>(::malloc(heap_size))),
   m_end(m_mem + heap_size),
   m_heap_size(heap_size),
   m_debugger(debugger),
   m_safe_mode(false)
{
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
  if (m_debugger) m_debugger->reset(m_mem, m_end);
}

void * fake_heap_t::calloc(std::size_t nmemb, std::size_t size)
{
  const unsigned chunk_size = size * nmemb;
  if (is_enough(size))
  {
    void * beg = m_cursor;
    uint8_t * end = m_cursor + chunk_size;
    memset(beg, 0, chunk_size);
    if (m_debugger) m_debugger->fake_calloc(beg, end, chunk_size);
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
    if (m_debugger) m_debugger->fake_malloc(beg, end, size);
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
  if (m_debugger) m_debugger->real_calloc(ptr, nmemb * size);
  return ptr;
}

void* fake_heap_t::cstdlib_malloc(size_t size)
{
  void * ptr = ::malloc(size);
  if (m_debugger) m_debugger->real_malloc(ptr, size);
  return ptr;
}

void fake_heap_t::free(void * ptr)
{
  if (ptr == nullptr) return;
  if (is_own(ptr))
  {
    // ничего не делаем, всё в порядке
    if (m_debugger) m_debugger->fake_free(ptr);
  }
  else
  {
    if (m_debugger) m_debugger->real_free(ptr);
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
  return size + (rest ? (m_heap_chunk_size - rest) : 0);
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
    if (get_debugger()) get_debugger()->fake_recalloc(beg, tail, sizeof(realloc_size_t), chunk_size, membs_size);
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
    if (get_debugger()) get_debugger()->fake_remalloc(beg, tail, sizeof(realloc_size_t), chunk_size, size);
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
      // ничего не делаем, всё в порядке
      if (get_debugger()) get_debugger()->fake_realloc(ptr, p8 + *chunk_size, size, *chunk_size, true);
      return ptr;
    }
    // текущий chunk размещался последним, достаточно просто увеличить его
    // размер подвинув курсор, но только, если достаточно памяти
    else if ((get_cursor() == (p8 + *chunk_size)) && is_enough(m_heap_chunk_size))
    {
      if (get_debugger()) get_debugger()->fake_realloc(p8, p8 + *chunk_size + m_heap_chunk_size, *chunk_size + m_heap_chunk_size, *chunk_size, false);
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
          get_debugger()->fake_realloc_move2fake(ptr, reptr, size, *(reinterpret_cast<realloc_size_t*>(reptr) - 1), *chunk_size);
        else
          get_debugger()->fake_realloc_move2heap(ptr, reptr, size, *chunk_size);
      }
      return reptr;
    }
  }
  else if (is_safe_mode())
  {
    // память блока размещена в куче (не хватало памяти)
    void * reptr = ::realloc(ptr, size);
    if (get_debugger()) get_debugger()->real_realloc(ptr, reptr, size);
    return reptr;
  }
  else
  {
    // памяти не хватило в fake-буфере, при этом safe_mode отключен
    throw std::bad_alloc();
  }
}

}

#include <cstdlib> // malloc, calloc, free, realloc
#include <cstring>
#include <cstdint> // uint8_t, uint32_t,...
#include <new> // std::bad_alloc
#include <iostream>



/*! \brief Фейковая куча
 *
 * Фейковая куча имеет реализацию методов ctsdlib-библиотеки : malloc, calloc,
 * realloc, free. Ведёт себя как "настоящая" куча, поддерживает размещение
 * блоков памяти в ограниченном заранее выделенном куске памяти, следит за
 * выравниванием, умеет в реаллокацию, имеет безопасный режим работы (когда
 * выделенный кусок памяти заканчивается - переключается на размещение данных в
 * куче). И вот это во всё :)
 *
 * Непотокобезопасна!
 */
class fake_heap_t
{
public:
  fake_heap_t(std::size_t heap_size, bool safe_mode, bool debug);
  ~fake_heap_t();

  /*! \brief Возвращает экземпляр объекта в исходное состояние
   *
   * Сбрасывает состояние объекта к исходному (неиспользованному), опустошает
   * выделенную память, "забывает" о размещённых блоках памяти.
   */
  void reset();

  /*! \brief Выделяет и зануляет память
   * \param nmemb Кол-во элементов размером size
   * \param size Размер элемента, октет
   * \return Указатель на блок памяти
   *
   * Выделяет память для массива из nmemb элементов по size байт каждый, и
   * возвращает указатель на выделенную память. Выделенная память обнуляется.
   */
  void * calloc(std::size_t nmemb, std::size_t size);

  /*! \brief Выделяет память
   * \param size Размер блока памяти, октет
   * \return Указатель на блок памяти
   *
   * Выделяет size байт и возвращает указатель на выделенную память. Выделенная
   * память не инициализируется.
   */
  void * malloc(size_t size);

  /*! \brief Изменяет размер блока динамической памяти
   * \param ptr Указатель на исходный блок памяти
   * \param size Размер нового блока памяти
   * \return Указатель на блока памяти
   *
   * Изменяет размер  блока  динамической  памяти, на который указывает ptr.
   * Новый размер блока будет равен size байт. Его содержимое останется
   * неизменным в промежутке вплоть до прежнего или нового размера, смотря что
   * меньше; добавленная память не инициализируется. Если ptr равен nullptr, то
   * результат вызова эквивалентен malloc(size); если size равен нулю, то
   * результат вызова эквивалентен free(ptr). Кроме случая, когда ptr равен
   * nullptr, он должен указывать на память, выделенную ранее посредством
   * malloc(), calloc() или realloc().
   */
  void * realloc(void *ptr, size_t size);

  /*! \brief Освобождает блок памяти
   * \param ptr Указатель на блок памяти
   *
   * Освобождает область памяти, на которую указывает ptr, которая быть выделена
   * ранее посредством malloc(), calloc() или realloc(). Иначе, если free(ptr)
   * был уже вызван ранее, результат операции не определен. Если ptr равен
   * nullptr, то ничего не происходит.
   */
  void free(void *ptr);

private:
  const std::size_t m_heap_size;
  const bool m_safe_mode;
  const bool m_debug;
  uint8_t * m_mem;
  uint8_t * m_cursor;
  uint8_t * m_end;

  inline bool is_own(const void * ptr) const { return (ptr >= m_mem) && (ptr < m_end); }
  inline bool is_enough(size_t size) const { return (m_cursor + 4 + size) <= m_end; }

  constexpr std::size_t mem_chunk() const { return 8; } // д.б. степерь двойки, не может быть меньше 8
  constexpr unsigned mem_chunk_mask() const { return 0xfffffff8; } // выравниваем по chunk-у (д.б. согласовано с размером chunk-а, не может быть меньше fffffff8)
  constexpr std::size_t mem_chunk_adoptation(std::size_t size)
  {
    return (size + mem_chunk()) & mem_chunk_mask();
  }
};


//------------------------------------------------------------------------------
// 14 Кб памяти с запасом для д.з., чанки по 128 октет (которых в достаточно для расширения realloc-ами)
// heap_size = 14 * 1024
// mem_chunk = 8 / 128
// mem_chunk_size = 0xfffffff8 / 0xffffff80
// mem_chunk_adoptaion = (size + mem_chunk) & mem_chunk_mask()
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// fake_heap_t
//------------------------------------------------------------------------------
fake_heap_t::fake_heap_t(std::size_t heap_size, bool safe_mode, bool debug)
  : m_heap_size(heap_size),
    m_safe_mode(safe_mode),
    m_debug(debug),
    m_mem((uint8_t*)::malloc(heap_size)),
    m_end(m_mem + heap_size)
{
}

fake_heap_t::~fake_heap_t()
{
  ::free(m_mem);
}

void fake_heap_t::reset()
{
  m_cursor = m_mem;
  if (m_debug)
  {
    std::cout << std::endl << "=== > fake_heap.reset from fake:" << m_cursor
              << " to fake:" << m_end << std::endl;
  }
}

void * fake_heap_t::calloc(size_t nmemb, size_t size)
{
  const unsigned chunk_size = mem_chunk_adoptation(size * nmemb);
  if (is_enough(chunk_size))
  {
    uint32_t * beg = (uint32_t*)m_cursor;
    uint8_t * end = m_cursor + 4 + chunk_size;
    *beg++ = (uint32_t)chunk_size;
    memset(beg, 0, (size + 7) & 0xfffffff8);
    if (m_debug)
    {
      std::cout << "=== > fake_heap.calloc from fake:" << beg << " to fake:"
                << end << ", size " << chunk_size << " (adopted from "
                << (unsigned)(size * nmemb) << ")" << std::endl;
    }
    m_cursor = end;
    return beg;
  }
  else if (m_safe_mode)
  {
    void * ptr = ::calloc(nmemb, size);
    if (m_debug)
    {
      std::cout << "=== > stdlib.calloc heap:" << ptr << " size "
                << (unsigned)(nmemb * size) << std::endl;
    }
    return ptr;
  }
  else // памяти не хватило в fake-буфере, при этом safe_mode отключен
  {
    throw std::bad_alloc;
  }
}

void * fake_heap_t::malloc(size_t size)
{
  const unsigned chunk_size = mem_chunk_adoptation(size);
  if (is_enough(chunk_size))
  {
    uint32_t * beg = (uint32_t*)m_cursor;
    uint8_t * end = m_cursor + 4 + chunk_size;
    *beg++ = (uint32_t)chunk_size;
    if (m_debug)
    {
      std::cout << "=== > fake_mem.malloc from fake:" << beg << " to fake:"
                << end << ", size " << chunk_size << " (adopted from "
                << (unsigned)size << std::endl;
    }
    m_cursor = end;
    return beg;
  }
  else if (m_safe_mode)
  {
    void * ptr = ::malloc(size);
    if (m_debug)
    {
      std::cout << "=== > stdlib.malloc heap:" << ptr << " size "
                << (unsinged)size << std::endl;
    }
    return ptr;
  }
  else // памяти не хватило в fake-буфере, при этом safe_mode отключен
  {
    throw std::bad_alloc;
  }
}

void * fake_heap_t::realloc(void * ptr, size_t size)
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
    uint8_t * p8 = (uint8_t*)ptr;
    uint32_t * chunk_size = ((uint32_t*)ptr) - 1;
    if (size <= *chunk_size) // новый фрагмент находится внутри выделенного ранее chunk-а
    {
      if (m_debug)
      {
        std::cout << "=== > fake_mem.realloc from fake:" << ptr << " to fake:"
                  << p8 + *chunk_size << ", size " << (unsigned)size
                  << " (stayed in chunk " << *chunk_size << ")" << std::endl;
      }
      return ptr;
    }
    else if ((m_cursor == (p8 + *chunk_size)) && // текущий chunk размещался последним, достаточно просто увеличить его размер подвинув курсор
             is_enough(FAKE_MEM_CHUNK))          // но только, если достаточно памяти
    {
      if (m_debug)
      {
        std::cout << "=== > fake_mem.realloc from fake:" << p8 << " to fake:"
                  << p8 + *chunk_size + FAKE_MEM_CHUNK << ", size "
                  << *chunk_size + FAKE_MEM_CHUNK << " (readopted from "
                  << *chunk_size << ")" << std::endl;
      }
      *chunk_size += FAKE_MEM_CHUNK;
      m_cursor += FAKE_MEM_CHUNK;
      return ptr;
    }
    else // освобождаем (забываем о существовании) предыдущего чанка и создаём новый
    {
      void * reptr = malloc(size);
      if (ptr == nullptr) return nullptr; // что, даже в куче места не хватило?
      memcpy(reptr, ptr, *chunk_size); // здесь возможно лишнее копирование, т.к. chunk_size м.б. больше чем size
      if (m_debug)
      {
        if (is_own(reptr))
          std::cout << "=== === > fake_mem.realloc moved from fake:" << ptr
                    << " to fake:" << reptr << ", new size "
                    << *(((uint32_t*)reptr) - 1) << " (adopted from "
                    << (unsigned)size << ", readopted from " << *chunk_size
                    << ")" << std::endl;
        else
          std::cout << "=== === > fake_mem.realloc moved from fake:" << ptr
                    << " to heap:" << reptr << ", new size " << (unsigned)size
                    << " (readopted from " << *chunk_size << ")" << std::endl;
      }
      return reptr;
    }
  }
  else if (m_safe_mode)
  {
    // память блока размещена в куче (не хватало памяти)
    void * reptr = ::realloc(ptr, size);
    if (m_debug)
    {
      std::cout << "=== > stdlib.realloc from heap:" << ptr << " to heap:"
                << reptr << " size " << size << std::endl;
    }
    return reptr;
  }
  else // памяти не хватило в fake-буфере, при этом safe_mode отключен
  {
    throw std::bad_alloc;
  }
}

void fake_heap_t::free(void * ptr)
{
  if (ptr == nullptr) return;
  if (is_own(ptr))
  {
    // ничего не делаем, всё в порядке
    if (m_debug)
    {
      fprintf(stderr, "=== > fake_mem.free fake:%p\n", ptr);
    }
  }
  else
  {
    if (m_debug)
    {
      fprintf(stderr, "=== > stdlib.free heap:%p\n", ptr);
    }
    // память блока размещена в куче (не хватало памяти)
    ::free(ptr);
  }
}


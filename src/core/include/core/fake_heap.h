// -*- mode: c++; coding: utf-8 -*-
/*! \file fake_heap.h
 * \brief "Фейковая куча", имеющая реализацию методов работы с динамической памятью
*/

#pragma once

#include <cstddef>
#include <cstdint>

#include <core/core.h>


/*! \addtogroup core
 * @{
*/
namespace core {

class fake_heap_debugger_t;

/*! \brief Фейковая куча
 *
 * Фейковая куча имеет реализацию методов cstdlib-библиотеки : malloc, calloc,
 * free. Реализация метода realloc выполняется с пом. \ref fake_realoc_heat_t.
 * Ведёт себя как "настоящая" куча, поддерживает размещение блоков памяти в
 * ограниченном заранее выделенном куске памяти, следит за выравниванием, имеет
 * безопасный режим работы (когда выделенный кусок памяти заканчивается -
 * переключается на размещение данных в куче). И вот это во всё :)
 *
 * Непотокобезопасна!
 */
class fake_heap_t
{
public:
  /*! \brief Конструктор
   * \param heap_size Размер куска памяти для размещения блоков памяти, октет
   * \param debugger Интерфейс отладчика кучи, для вывода сообщений в поток
   */
  fake_heap_t(
    std::size_t heap_size,
    fake_heap_debugger_t* debugger = nullptr);

  /*! \brief Деструктор
   */
  virtual ~fake_heap_t();

  /*! \brief Возвращает экземпляр объекта в исходное состояние
   *
   * Сбрасывает состояние объекта к исходному (неиспользованному), опустошает
   * выделенную память, "забывает" о размещённых блоках памяти. Вызов методов
   * free после вызова метода reset не требуется.
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
  virtual void * calloc(std::size_t nmemb, std::size_t size);

  /*! \brief Выделяет память
   * \param size Размер блока памяти, октет
   * \return Указатель на блок памяти
   *
   * Выделяет size байт и возвращает указатель на выделенную память. Выделенная
   * память не инициализируется.
   */
  virtual void * malloc(size_t size);

  /*! \brief Освобождает блок памяти
   * \param ptr Указатель на блок памяти
   *
   * Освобождает область памяти, на которую указывает ptr, которая быть выделена
   * ранее посредством malloc(), calloc() или realloc(). Иначе, если free(ptr)
   * был уже вызван ранее, результат операции не определен. Если ptr равен
   * nullptr, то ничего не происходит.
   */
  void free(void *ptr);

  /*! \brief Возвращает размер фейковой кучи
   * \return Размер куска памяти для размещения блоков памяти, октет
  */
  inline std::size_t get_heap_size() const { return m_heap_size; }

  /*! \brief Устанавливает флаг использования кучи, если в фейковой не хватило памяти */
  inline void enable_safe_mode() { m_safe_mode = true; }
  /*! \brief Сбрасывает флаг использования кучи, если в фейковой не хватило памяти */
  inline void disable_safe_mode() { m_safe_mode = false; }
  /*! \brief Возвращает признак использования кучи */
  inline bool is_safe_mode() const { return m_safe_mode; }

  /*! \brief Подключает отладчик кучи, для вывода сообщений в поток */
  inline void attach_debugger(fake_heap_debugger_t* debugger) { m_debugger = debugger; }
  /*! \brief Отключает отладчик кучи, для вывода сообщений в поток */
  inline void detach_debugger() { m_debugger = nullptr; }

protected:
  inline bool is_own(const void* ptr) const { return (ptr >= m_mem) && (ptr < m_end); }
  inline bool is_enough(size_t size) const { return (m_cursor + size) <= m_end; }
  inline fake_heap_debugger_t* get_debugger() { return m_debugger; }
  inline uint8_t* get_cursor() { return m_cursor; }
  inline void move_cursor(uint8_t* to) { m_cursor = to; }

  void* cstdlib_calloc(std::size_t nmemb, std::size_t size);
  void* cstdlib_malloc(size_t size);

private:
  uint8_t* m_mem;
  uint8_t* m_cursor;
  uint8_t* m_end;

  const std::size_t m_heap_size;
  fake_heap_debugger_t* m_debugger;
  bool m_safe_mode;

  fake_heap_t() = delete;
  fake_heap_t(const fake_heap_t&) = delete;
};

/*! \brief Фейковая куча с поддержкой реаллокации
 *
 * Если стратегия использования fake_heap подразумевает частое использование
 * метода realloc, то рекомендуется выбрать размер chunk_size "с запасом", с
 * тем, чтобы realloc не перемещал данные, а мог расшириться в ранее
 * выделенном блоке памяти.
 */
class fake_realloc_heap_t : public fake_heap_t
{
public:
  /*! \brief Конструктор
   * \param heap_size Размер куска памяти для размещения блоков памяти, октет
   * \param chunk_size Кол-во октет для выравнивания chunk-ов фейковой кучи
   * \param debugger Интерфейс отладчика кучи, для вывода сообщений в поток
   *
   * Параметр chunk_size рекомендуется указывать кратным длине слова на
   * целевой платформе. Следует учитывать, что вместе с чанком "резервируется"
   * пространство размером в 4 октет для размещения информации о реалокации.
   * Таким образом, размер heap_size не м.б. меньше 4 + chunk_size.
   *
   * Если стратегия использования fake_heap подразумевает частое использование
   * метода realloc, то рекомендуется выбрать размер chunk_size "с запасом", с
   * тем, чтобы realloc не перемещал данные, а мог расшириться в ранее
   * выделенном блоке памяти.
   */
  fake_realloc_heap_t(
    std::size_t heap_size,
    std::size_t chunk_size,
    fake_heap_debugger_t* debugger = nullptr);

  /*! \brief Выделяет и зануляет память
   * \param nmemb Кол-во элементов размером size
   * \param size Размер элемента, октет
   * \return Указатель на блок памяти
   *
   * Выделяет память для массива из nmemb элементов по size байт каждый, и
   * возвращает указатель на выделенную память. Выделенная память обнуляется.
   */
  virtual void * calloc(std::size_t nmemb, std::size_t size) override;

  /*! \brief Выделяет память
   * \param size Размер блока памяти, октет
   * \return Указатель на блок памяти
   *
   * Выделяет size байт и возвращает указатель на выделенную память. Выделенная
   * память не инициализируется.
   */
  virtual void * malloc(size_t size) override;

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

private:
  const std::size_t m_heap_chunk_size;

  using realloc_size_t = uint32_t;

  fake_realloc_heap_t() = delete;
  fake_realloc_heap_t(const fake_realloc_heap_t&) = delete;

  unsigned mem_chunk_adoptation(std::size_t size) const;
};

}

/*! @} */

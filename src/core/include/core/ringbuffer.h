// -*- mode: c++; coding: utf-8 -*-
#pragma once

#include <array>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <stdexcept>

#include <core/fake_heap.h>


namespace core {

template <class T, size_t Size>
struct ringbuffer_ring_t
{
  std::array<T, Size> data;
  std::atomic<unsigned> head{ 0 };
  std::atomic<unsigned> tail{ 0 };

  [[nodiscard]] constexpr inline unsigned next(unsigned current) const noexcept
  {
    return (current + 1) % Size;
  }

  [[nodiscard]] constexpr unsigned head_relaxed() const noexcept
  {
    return head.load(std::memory_order_relaxed);
  }

  [[nodiscard]] constexpr unsigned tail_relaxed() const noexcept
  {
    return tail.load(std::memory_order_relaxed);
  }

  /*! \brief Добавление элемента в кольцевой буфер
   * \param value Новый элемент
   * \return Признак успешного добавления элемента в буфер, иначе буфер полон
   */
  [[nodiscard]] bool push(const T& value) noexcept
  {
    unsigned _head = head.load(std::memory_order_relaxed);
    unsigned next_head = next(_head);
    if (next_head == tail.load(std::memory_order_acquire))
      return false;
    data[_head] = value;
    head.store(next_head, std::memory_order_release);
    return true;
  }

  /*! \brief Выборка элемента из буфера
   * \param value Выбранный элемент
   * \return Признак успешной выборки элемента из буфера, иначе буфер пуст
   */
  [[nodiscard]] bool pop(T& value) noexcept
  {
    unsigned _tail = tail.load(std::memory_order_relaxed);
    if (_tail == head.load(std::memory_order_acquire))
      return false;
    value = std::move(data[_tail]);
    tail.store(next(_tail), std::memory_order_release);
    return true;
  }
};

template <class T, size_t Size>
class ringbuffer_iterator_t
{
public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = T;
  using difference_type = ptrdiff_t;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;

  using ring_t = ringbuffer_ring_t<T, Size>;
  enum class category_t { begin, end }; // если две кагории итераторов - на двигующиеся начало и двигающийся конец

  constexpr ringbuffer_iterator_t() noexcept { }
  constexpr explicit ringbuffer_iterator_t(ring_t* ring, category_t cat) noexcept : _ring(ring), _category(cat), _known(false) { }

  [[nodiscard]] reference operator*() noexcept
  {
    return *operator->();
  }

  [[nodiscard]] pointer operator->() noexcept
  {
    if (!_known)
    {
      _known = _ring->pop(_item);
      assert(_known); // "cannot dereference missing ringbuffer iterator"
    }
    return &_item;
  }

  constexpr ringbuffer_iterator_t& operator++() noexcept {
    _known = _ring->pop(_item);
    return *this;
  }

  constexpr ringbuffer_iterator_t operator++(int) noexcept {
    ringbuffer_iterator_t _tmp = *this;
    ++* this;
    return _tmp;
  }

  [[nodiscard]] constexpr bool operator==(const ringbuffer_iterator_t& right) const noexcept {
    _compatible(right);
    if (_category == right._category)
      return true;
    if (_category == category_t::begin)
    {
      if (_ring->head_relaxed() == _ring->tail_relaxed())
        return !_known; // это не end, пока не выгрузились данные из итератора
    }
    else // if (right._category == category_t::begin)
    {
      if (right._ring->head_relaxed() == right._ring->tail_relaxed())
        return !right._known; // это не end, пока не выгрузились данные из итератора
    }
    return false;
  }

private:
  constexpr void _compatible(const ringbuffer_iterator_t& right) const noexcept
  {
    assert(_ring == right._ring); // "ringbuffer iterators incompatible"
  }

  ring_t* _ring;
  category_t _category; // если end, то iterator указывает на конец кольцевого буфера (это перманентное место, его не существует)
  value_type _item;
  bool _known;
};

template <class T, size_t Size>
[[nodiscard]] constexpr bool operator==(const ringbuffer_iterator_t<T, Size>&left, const ringbuffer_iterator_t<T, Size>&right) {
  return left.operator==(right);
}

template <class T, size_t Size>
[[nodiscard]] constexpr bool operator!=(const ringbuffer_iterator_t<T, Size>& left, const ringbuffer_iterator_t<T, Size>& right) {
  return !(left.operator==(right));
}


/*! \brief Потокобезопасный кольцевой буфер на базе оптимистического блокирования
 *
 * Кольцевой буфер с оптимистическим механизмом блокирования для передачи данных от
 * генерирующего потока к потребляющему потоку без каких-либо блокировок. Операции
 * над структурой данных являются "оптимистическими" и завершаются в течении
 * константного количество шагов. Это позволяет исопльзовать структуру данных в
 * жёстких системах реального времени или для коммуникации с обработчиками прерываний
 * или сигналов.
 *
 * Внимание! В буфер не влезает более чем Size-1 элементов.
 */
template<typename T, size_t Size, class _Alloc = std::allocator<T>>
class ringbuffer_t
{
private:
  using ring_t = ringbuffer_ring_t<T, Size>;
  using _rebind_alloc = typename std::allocator_traits<_Alloc>::template rebind_alloc<ring_t>;

  _Alloc _alloc;
  ring_t* const _ring{0};

public:
  using value_type = T;
  using allocator_type = _Alloc;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = typename std::allocator_traits<allocator_type>::size_type;
  using difference_type = typename std::allocator_traits<allocator_type>::difference_type;

  using iterator = ringbuffer_iterator_t<T, Size>;
  //using const_iterator = ringbuffer_const_iterator_t<T, Size>;

public:
  inline ringbuffer_t() noexcept : _alloc(_Alloc()), _ring(_construct(_alloc)) { }
  inline explicit ringbuffer_t(const _Alloc& alloc) noexcept : _alloc(alloc), _ring(_construct(_alloc)) { }
  ~ringbuffer_t() noexcept { _destruct(_ring); }

public:
  /*! \brief Добавление элемента в кольцевой буфер
   * \param value Новый элемент
   * \return Признак успешного добавления элемента в буфер, иначе буфер полон
   */
  bool push(const T& value) noexcept
  {
    return _ring->push(value);
  }

  /*! \brief Добавление элемента в кольцевой буфер
   * \param value Новый элемент
   * \return Признак успешного добавления элемента в буфер, иначе буфер полон
   *
   * Странно... не удалось воспользоваться универсальным указателем здесь.
   */
  inline bool push(const T* value)
  {
    if (!value)
      throw std::invalid_argument("Null pointer while adding an item to the ringbuffer");
    return push(*value);
  }

  /*! \brief Выборка элемента из буфера
   * \param value Выбранный элемент
   * \return Признак успешной выборки элемента из буфера, иначе буфер пуст
   */
  bool pop(T& value) noexcept
  {
    return _ring->pop(value);
  }

public:
  [[nodiscard]] constexpr iterator begin() noexcept
  {
    return iterator(_ring, iterator::category_t::begin);
  }

  [[nodiscard]] constexpr iterator end() noexcept
  {
    return iterator(_ring, iterator::category_t::end);
  }

  [[nodiscard]] constexpr size_type max_size() const noexcept { return Size; }

  // Не следует использовать этот метод! (он здесь ради примера)
  [[nodiscard]] constexpr bool empty() const noexcept
  {
    return _ring->head_relaxed() == _ring->tail_relaxed();
  }

private:
  ringbuffer_t(const ringbuffer_t&) = delete;
  const ringbuffer_t& operator=(const ringbuffer_t&) = delete;

  [[nodiscard]] ring_t* _construct(const _Alloc& alloc)
  {
    _rebind_alloc ralloc(_alloc);
    ring_t* ring = std::allocator_traits<_rebind_alloc>::allocate(ralloc, 1);
    std::allocator_traits<_rebind_alloc>::construct(ralloc, ring);
    return ring;
  }

  void _destruct(ring_t* ring)
  {
    _rebind_alloc ralloc(_alloc);
    std::allocator_traits<_rebind_alloc>::destroy(ralloc, ring);
    std::allocator_traits<_rebind_alloc>::deallocate(ralloc, ring, 1);
  }
};

}

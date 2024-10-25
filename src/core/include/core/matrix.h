﻿// -*- mode: c++; coding: utf-8 -*-
/*! \file matrix.h
 * \brief Реализация 2х-мерной разреженной бесконечной матрицы
*/

#pragma once

#include <list>
#include <algorithm>
#include <functional>
#include <tuple>

#include <core/core.h>


/*! \addtogroup core
 * @{
*/
namespace core
{

/*! \brief "Заглушка" шаблонизируемого параметра, реализующая copy/direct initialization
*
* Предназначена для перегрузки оператора тип() по умолчанию для приведения типа
* по умолчанию при чтении значений из "заглушки". Имеет интерфейс изменения
* данных, ассоциированных с "заглушкой", вызывает callback при обращении к
* assignment оператору.
*/
template<typename T>
struct matrix_stub_t
{
  using on_assign_t = std::function<matrix_stub_t<T>(const T&)>;
  explicit matrix_stub_t(T value, on_assign_t on_assign) : stub(value), on_assign(on_assign) { }
  matrix_stub_t<T> operator=(const T& other) { stub = other; return on_assign(other); }
  operator T() const { return stub; }
private:
  T stub;
  on_assign_t on_assign;
};

/*! \brief Элемент, хранящийся в контейнере матрицы
*
* Имеет два ключа - адреса ячеек матрицы, и значение ячейки.
*/
template<typename T>
struct matrix_item_t
{
  unsigned key1, key2;
  T data;
  matrix_item_t(unsigned key1, unsigned key2, const T& data) : key1(key1), key2(key2), data(data) { }
};

/*! \brief Контейнер матрицы
*/
template<typename T, class _Alloc>
struct matrix_container_t
{
  using items_t = std::list<matrix_item_t<T>, _Alloc>;
  items_t data;
  const T nothing;
  matrix_container_t(T default_value, const _Alloc & alloc) : nothing(default_value), data(alloc) { }
};

/*! \brief Константный итератор матрицы
*/
template<typename T, class _Alloc>
struct matrix_const_iterator_t
{
public:
  using _Container = const typename matrix_container_t<T, _Alloc>::items_t;
  using _Container_iterator = typename _Container::const_iterator;

  using iterator_category = std::forward_iterator_tag;
  using value_type = std::tuple<unsigned, unsigned, T>;
  using difference_type = ptrdiff_t;
  using pointer = value_type;
  using reference = value_type;

  constexpr matrix_const_iterator_t() noexcept { }
  constexpr explicit matrix_const_iterator_t(_Container_iterator itr) noexcept : _itr(itr) { }

  [[nodiscard]] reference operator*() noexcept
  {
    return operator->();
  }
  [[nodiscard]] pointer operator->() noexcept
  {
    return std::make_tuple(_itr->key1, _itr->key2, _itr->data);
  }

  matrix_const_iterator_t& operator++() noexcept
  {
    ++_itr;
    return *this;
  }
  matrix_const_iterator_t operator++(int) noexcept
  {
    matrix_const_iterator_t _Tmp = *this;
    ++* this;
    return _Tmp;
  }

  [[nodiscard]] bool operator==(const matrix_const_iterator_t& right) const noexcept { return this->_itr == right._itr; }
  [[nodiscard]] bool operator!=(const matrix_const_iterator_t& right) const noexcept { return !(*this == right); }

private:
  _Container_iterator _itr;
};

#if 0
/*! \brief Итератор матрицы
*
* Функцинал не const итераторов не поддерживается (данные в матрице через
* итераторы не меняются) - этого не требовалось по условию задачи, ну а
* делать больше, чем необходимо лень (знаю, что справлюсь, и этого достаточно).
*/
template<typename T, class _Alloc>
struct matrix_iterator_t
{
public:
  using _Mybase = matrix_const_iterator_t<T, _Alloc>;
  using _Container = typename matrix_container_t<T, _Alloc>::items_t;
  using _Container_iterator = typename _Container::iterator;

  using iterator_category = std::forward_iterator_tag;
  using value_type = std::tuple<unsigned, unsigned, T>;
  using difference_type = ptrdiff_t;
  using pointer = value_type;
  using reference = value_type;

  constexpr matrix_iterator_t() noexcept { }
  constexpr explicit matrix_iterator_t(_Container_iterator itr) noexcept : _itr(itr) { }

  [[nodiscard]] reference operator*() noexcept
  {
    return operator->();
  }
  [[nodiscard]] pointer operator->() noexcept
  {
    return std::make_tuple(_itr->key1, _itr->key2, _itr->data);
  }

  matrix_iterator_t& operator++() noexcept
  {
    ++_itr;
    return *this;
  }
  matrix_iterator_t operator++(int a) noexcept
  {
    matrix_const_iterator_t _Tmp = *this;
    ++* this;
    return _Tmp;
  }

  [[nodiscard]] bool operator==(const matrix_iterator_t& right) const noexcept { return this->_itr == right._itr; }
  [[nodiscard]] bool operator!=(const matrix_iterator_t& right) const noexcept { return !(*this == right); }

private:
  _Container_iterator _itr;
};
#endif

/*! \brief 2х-мерная разреженная бесконечная матрица
*
* 2-мерную разреженная бесконечная матрица, заполненная значениями по умолчанию.
* Матрица хранит только занятые элементы - значения которых хотя бы раз
* присваивались. Присвоение в ячейку значения по умолчанию освобождает ячейку.
* Умеет сообщать - сколько ячеек реально занято? Умеет проходить по всем занятым
* ячейкам (порядок не имеет значения, но в текущей реализации - соответствует
* порядку добавления). Итераторы возвращают позицию ячейки и ее значение. При
* чтении элемента из свободной ячейки возвращает значение по умолчанию.
*/
template<typename T, T _Value, class _Alloc = std::allocator<T>, unsigned _Dimension = 2>
class matrix_t
{
  using _Container_allocator = typename std::allocator_traits<_Alloc>::template rebind_alloc<matrix_item_t<T>>;
  using _Container = matrix_container_t<T, _Container_allocator>;
  using _rebind_alloc = typename std::allocator_traits<_Alloc>::template rebind_alloc<_Container>;

public:
  using value_type = T;
  using allocator_type = _Alloc;
  using size_type = typename std::allocator_traits<allocator_type>::size_type;
  using const_reference = const T&;

  using const_iterator = matrix_const_iterator_t<T, _Container_allocator>;

  matrix_t<T, _Value, allocator_type, _Dimension>() :
    _alloc(allocator_type()),
    _container_alloc(_Container_allocator(_alloc)),
    _container(_construct()) { }
  matrix_t<T, _Value, allocator_type, _Dimension>(const allocator_type& alloc) :
    _alloc(alloc),
    _container_alloc(_Container_allocator(_alloc)),
    _container(_construct()) { }
  ~matrix_t<T, _Value, allocator_type, _Dimension>() { _destruct(_container); }

  [[nodiscard]] matrix_t<T, _Value, allocator_type, _Dimension - 1> operator[](size_type idx)
  {
    return matrix_t<T, _Value, allocator_type, _Dimension - 1>(_container, idx);
  }
  [[nodiscard]] const matrix_t<T, _Value, allocator_type, _Dimension - 1> operator[](size_type idx) const
  {
    return matrix_t<T, _Value, allocator_type, _Dimension - 1>(_container, idx);
  }

  constexpr size_type size() const noexcept { return _container->data.size(); }
  [[nodiscard]] constexpr const_iterator begin() const noexcept { return const_iterator( _container->data.begin()); }
  [[nodiscard]] constexpr const_iterator end() const noexcept { return const_iterator(_container->data.end()); }
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return begin(); }
  [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }

  #if 0 // функцинал не const итераторов не поддерживается (данные в матрице через итераторы не меняются)
  using iterator = matrix_iterator_t<T, _Alloc>;
  [[nodiscard]] constexpr iterator begin() noexcept { return iterator(_container->data.begin()); }
  [[nodiscard]] constexpr iterator end() noexcept { return iterator(_container->data.end()); }
  #else
  using iterator = const_iterator;
  #endif

private:
  allocator_type _alloc;
  _Container_allocator _container_alloc;
  _Container* _container;

  [[nodiscard]] _Container* _construct()
  {
    static_assert(_Dimension == 2, "It's impossible to create matrix with a dimension greater than 2");
    _rebind_alloc ralloc(_alloc);
    _Container* mtx = std::allocator_traits<_rebind_alloc>::allocate(ralloc, 1);
    std::allocator_traits<_rebind_alloc>::construct(ralloc, mtx, _Value, _container_alloc);
    return mtx;
  }

  void _destruct(_Container* mtx)
  {
    _rebind_alloc ralloc(_alloc);
    std::allocator_traits<_rebind_alloc>::destroy(ralloc, mtx);
    std::allocator_traits<_rebind_alloc>::deallocate(ralloc, mtx, 1);
  }
};

/*! \brief 2х-мерная разреженная бесконечная матрица (1й уровень)
*
* Экземпляр класса не предпоалагется создавать и использовать иначе, как из
* шаблона matrix_t<T, _Value, _Alloc, _Dimension>.
*/
template<typename T, T _Value, class _Alloc>
class matrix_t<T, _Value, _Alloc, 1>
{
public:
  using value_type = T;
  using const_reference = const T&;
  using allocator_type = _Alloc;
  using size_type = typename std::allocator_traits<allocator_type>::size_type;

  ~matrix_t<T, _Value, allocator_type, 1>() = default;

  const_reference operator[](size_type idx2) const
  {
    const auto& ref = _container->data;
    size_type idx1 = this->_key1;
    typename _Container::items_t::const_iterator itr = std::find_if(
      ref.begin(),
      ref.end(),
      [idx1, idx2](const matrix_item_t<value_type>& itm) -> bool { return itm.key1 == idx1 && itm.key2 == idx2; }
    );
    if (itr == ref.cend())
      return _container->nothing;
    return itr->data;
  }

  matrix_stub_t<value_type> operator[](size_type idx2)
  {
    auto& ref = _container->data;
    size_type idx1 = this->_key1;
    _Container_iterator itr = std::find_if(
      ref.begin(),
      ref.end(),
      [idx1, idx2](const matrix_item_t<value_type>& itm) -> bool { return itm.key1 == idx1 && itm.key2 == idx2; }
    );
    if (itr == ref.cend())
    {
      auto on_add = [this, idx1, idx2](const_reference value) -> matrix_stub_t<value_type> { return do_add(idx1, idx2, value); };
      return matrix_stub_t<value_type>(_Value, on_add);
    }
    else
    {
      auto on_modify = [this, itr](const_reference value) -> matrix_stub_t<value_type> { return do_modify(itr, value); };
      return matrix_stub_t<value_type>(itr->data, on_modify);
    }
  }

private:
  using _Container_allocator = typename std::allocator_traits<_Alloc>::template rebind_alloc<matrix_item_t<T>>;
  using _Container = matrix_container_t<value_type, _Container_allocator>;
  using _Container_iterator = typename _Container::items_t::iterator;
  _Container* _container;
  size_type _key1;

  // доступ к конструктору с параметром из matrix_t с _Dimension==2
  friend matrix_t<T, _Value, allocator_type, 2>;
  matrix_t<T, _Value, allocator_type, 1>(_Container* self, size_type N) : _container(self), _key1(N) { }

  // запрещаю напрямую создавать матрицу с измерением _Dimension=1
  matrix_t<T, _Value, allocator_type, 1>() = delete;
  const matrix_t<T, _Value, allocator_type, 1>& operator=(const matrix_t<T, _Value, allocator_type, 1>&) = delete;

  // метод добавления новых данных в контейнер
  matrix_stub_t<value_type> do_add(size_type idx1, size_type idx2, const_reference value)
  {
    if (value != _Value)
    {
      _Container_iterator itr = _container->data.insert(_container->data.end(), matrix_item_t<value_type>(idx1, idx2, value));
      auto on_modify = [this, itr](const_reference value) -> matrix_stub_t<value_type> { return do_modify(itr, value); };
      return matrix_stub_t<value_type>(itr->data, on_modify);
    }
    else
    {
      auto on_add = [this, idx1, idx2](const_reference value) -> matrix_stub_t<value_type> { return do_add(idx1, idx2, value); };
      return matrix_stub_t<value_type>(_Value, on_add);
    }
  }

  // метод изменения существующих данных в контейнере, или удаления значением по умолчанию
  matrix_stub_t<value_type> do_modify(_Container_iterator itr, const_reference value)
  {
    if (value != _Value)
    {
      itr->data = value;
      auto on_modify = [this, itr](const_reference value) -> matrix_stub_t<value_type> { return do_modify(itr, value); };
      return matrix_stub_t<value_type>(itr->data, on_modify);
    }
    else
    {
      size_type idx1 = itr->key1;
      size_type idx2 = itr->key2;
      _container->data.erase(itr);
      auto on_add = [this, idx1, idx2](const_reference value) -> matrix_stub_t<value_type> { return do_add(idx1, idx2, value); };
      return matrix_stub_t<value_type>(_Value, on_add);
    }
  }
};

}

/*! @} */

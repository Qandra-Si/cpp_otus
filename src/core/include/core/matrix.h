// -*- mode: c++; coding: utf-8 -*-

#include <list>
#include <algorithm>
#include <functional>
#include <tuple>


template<typename T>
struct stub_t
{
  using on_assign_t = std::function<const T&(const T&)>;
  explicit stub_t(T value, on_assign_t on_assign) : stub(value), on_assign(on_assign) { std::cout << "<direct>"; }
  void operator=(const T& other) { std::cout << "<assign>"; stub = on_assign(other); }
  operator T() { std::cout << "<copy>"; return stub; }
private:
  T stub;
  on_assign_t on_assign;
};

template<typename T>
struct matrix_item_t
{
  unsigned key1, key2;
  T data;
  matrix_item_t(unsigned key1, unsigned key2, const T& data) : key1(key1), key2(key2), data(data) { }
};

template<typename T>
struct matrix_container_t
{
  using items_t = std::list<matrix_item_t<T>>;
  items_t data;
};

template<typename T>
struct matrix_const_iterator_t
{
public:
  using _Container = const typename matrix_container_t<T>::items_t;
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

#if 0 // функцинал не const итераторов не поддерживается (данные в матрице через итераторы не меняются)
template<typename T>
struct matrix_iterator_t
{
public:
  using _Mybase = matrix_const_iterator_t<T>;
  using _Container = typename matrix_container_t<T>::items_t;
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

template<typename T, T _Value, class _Alloc = std::allocator<T>, unsigned _Dimension = 2>
class matrix_t
{
  using _Container = matrix_container_t<T>;
  using _rebind_alloc = typename std::allocator_traits<_Alloc>::template rebind_alloc<_Container>;

public:
  using value_type = T;
  using allocator_type = _Alloc;
  using size_type = typename std::allocator_traits<allocator_type>::size_type;
  using const_reference = const T&;

  using const_iterator = matrix_const_iterator_t<T>;

  matrix_t<T, _Value, allocator_type, _Dimension>() : _alloc(allocator_type()), _container(_construct())
  {
    std::cout << "[ctor " << _Dimension << "]";
  }
  matrix_t<T, _Value, allocator_type, _Dimension>(const allocator_type& alloc) : _alloc(alloc), _container(_construct())
  {
    std::cout << "[ctor " << _Dimension << "]";
  }
  ~matrix_t<T, _Value, allocator_type, _Dimension>() {
    std::cout << "[dtor " << _Dimension  << "]";
    _destruct(_container);
  }

  [[nodiscard]] matrix_t<T, _Value, allocator_type, _Dimension - 1> operator[](size_type idx)
  {
    std::cout << "[proxy " << _Dimension << "]";
    return matrix_t<T, _Value, allocator_type, _Dimension - 1>(_container, idx);
  }

  [[nodiscard]] const matrix_t<T, _Value, allocator_type, _Dimension - 1> operator[](size_type idx) const
  {
    std::cout << "[proxy " << _Dimension << "]";
    return matrix_t<T, _Value, allocator_type, _Dimension - 1>(_container, idx);
  }

  constexpr size_type size() const noexcept { return _container->data.size(); }
  [[nodiscard]] constexpr const_iterator begin() const noexcept { return const_iterator( _container->data.begin()); }
  [[nodiscard]] constexpr const_iterator end() const noexcept { return const_iterator(_container->data.end()); }
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return begin(); }
  [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }

  #if 0 // функцинал не const итераторов не поддерживается (данные в матрице через итераторы не меняются)
  using iterator = matrix_iterator_t<T>;
  [[nodiscard]] constexpr iterator begin() noexcept { return iterator(_container->data.begin()); }
  [[nodiscard]] constexpr iterator end() noexcept { return iterator(_container->data.end()); }
  #else
  using iterator = const_iterator;
  #endif

private:
  allocator_type _alloc;
  _Container* _container;

  [[nodiscard]] _Container* _construct()
  {
    static_assert(_Dimension == 2, "It's impossible to create matrix with a dimension greater than 2");
    _rebind_alloc ralloc(_alloc);
    _Container* mtx = std::allocator_traits<_rebind_alloc>::allocate(ralloc, 1);
    std::allocator_traits<_rebind_alloc>::construct(ralloc, mtx);
    return mtx;
  }

  void _destruct(_Container* mtx)
  {
    _rebind_alloc ralloc(_alloc);
    std::allocator_traits<_rebind_alloc>::destroy(ralloc, mtx);
    std::allocator_traits<_rebind_alloc>::deallocate(ralloc, mtx, 1);
  }
};

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
    std::cout << "{const_reference 1}";
    const auto& ref = _container->data;
    size_type idx1 = this->_key1;
    _Container::items_t::const_iterator itr = std::find_if(
      ref.begin(),
      ref.end(),
      [idx1, idx2](const matrix_item_t<value_type>& itm) -> bool { return itm.key1 == idx1 && itm.key2 == idx2; }
    );
    if (itr == ref.cend())
      return this->nothing;
    return itr->data;
  }

  stub_t<T> operator[](size_type idx2)
  {
    std::cout << "{reference to stub 1}";
    auto& ref = _container->data;
    size_type idx1 = this->_key1;
    _Container_iterator itr = std::find_if(
      ref.begin(),
      ref.end(),
      [idx1, idx2](const matrix_item_t<value_type>& itm) -> bool { return itm.key1 == idx1 && itm.key2 == idx2; }
    );
    if (itr == ref.cend())
    {
      auto on_add = [this, idx1, idx2](const_reference value) -> const_reference { return do_add(idx1, idx2, value); };
      return stub_t<T>(_Value, on_add);
    }
    else
    {
      auto on_modify = [this, itr](const_reference value) -> const_reference { return do_modify(itr, value); };
      return stub_t<T>(itr->data, on_modify);
    }
  }

private:
  using _Container = matrix_container_t<T>;
  using _Container_iterator = typename _Container::items_t::const_iterator;
  _Container* _container;
  size_type _key1;
  static const value_type nothing{ _Value };

  // доступ к конструктору с параметром из matrix_t с _Dimension==2
  friend matrix_t<T, _Value, allocator_type, 2>;
  matrix_t<T, _Value, allocator_type, 1>(_Container* self, size_type N) : _container(self), _key1(N) { }

  // запрещаю напрямую создавать матрицу с измерением _Dimension=1
  matrix_t<T, _Value, allocator_type, 1>() = delete;
  const matrix_t<T, _Value, allocator_type, 1>& operator=(const matrix_t<T, _Value, allocator_type, 1>&) = delete;

  // метод добавления новых данных в контейнер
  const_reference do_add(size_type idx1, size_type idx2, const_reference value)
  {
    auto& ref = _container->data;
    ref.emplace_back(matrix_item_t<value_type>(idx1, idx2, value));
    return ref.back().data;
  }
  // метод изменения существующих данных в контейнере, или удаления значением по умолчанию
  const_reference do_modify(_Container_iterator itr, const_reference value)
  {
    if (value != _Value)
      return value;
    else
    {
      auto& ref = _container->data;
      ref.erase(itr);
      return this->nothing;
    }
  }
};


#include <cpp_otus_config.h>
template<class T>
void foo(T)
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
}

int main()
{
  {
    const matrix_t<int, -1> matrix; std::cout << std::endl;
    std::cout << "matrix.size() == " << matrix.size() << std::endl;
    std::cout << "matrix[0][1] == " << matrix[0][1] << std::endl;
    //matrix[2][2] = 33; // невозможно присваивать значения переменной, которая объявлена как константа
  } std::cout << std::endl;

  {
    matrix_t<int, -1> matrix; std::cout << std::endl;
    std::cout << "matrix.size() == " << matrix.size() << std::endl;
    std::cout << "matrix[2][2] == " << matrix[2][2] << std::endl;
    matrix[2][2] = 33;
  } std::cout << std::endl;

  matrix_t<int, -1> matrix; std::cout << std::endl;
  std::cout << "matrix.size() == " << matrix.size() << std::endl;
  auto a = matrix[0][0]; std::cout << std::endl; // [0]
  std::cout << "matrix[0][0] == " << a << std::endl;
  std::cout << "matrix.size() == " << matrix.size() << std::endl;

  matrix[100][100] = 314;
  std::cout << "matrix[100][100] == " << matrix[100][100] << std::endl;
  std::cout << "matrix.size() == " << matrix.size() << std::endl;

  // выведется одна строка
  // 100100314
  for (auto c : matrix)
  {
    unsigned x;
    unsigned y;
    int v;
    std::tie(x, y, v) = c;
    std::cout << x << y << v << std::endl;
  }

  matrix[100][100] = -1;

  for (matrix_t<int, -1>::iterator itr = matrix.begin(), end = matrix.end(); itr != end; ++itr)
  {
    unsigned x;
    unsigned y;
    int v;
    std::tie(x, y, v) = *itr;
    std::cout << x << y << v << std::endl;
  }

  return 0;
}

// Сделать "типовую реализацию" двухмерной матрицы с proxy-классом, в котором были бы переопределены operator[]
// которая гуглится за 1 минуту,... было скучно. Поэтому пошёл своим путём, где контейнер matrix_t являет собой
// шаблон с шаблонными методами, имеющими циклическую реализацию. Так было интересно поупражняться в
// метапрограммировании.

// -*- mode: c++; coding: utf-8 -*-

#include <iostream>


struct stub_t
{
  explicit stub_t(int value) : value(value) { std::cout << "<direct> "; }
  void operator=(const int& other) { std::cout << "<assign> "; value = other; }
  operator int() { std::cout << "<copy> "; return value; }

  // тут можно, конечно, ещё реализовать сравнение, но это не освободит от необходимости реализовать operator int()
  //bool operator==(const int& other) { std::cout << "<compare> "; return value == other; }
private:
  int value;
};

template<typename T>
struct matrix_container_t
{
  T data[3][3];
  std::size_t allocated{ 0 };
};

template<typename T, T _Value, class _Alloc = std::allocator<T>, unsigned _Dimension = 2>
class matrix_t
{
  using _container_type = matrix_container_t<T>;
  using _rebind_alloc = typename std::allocator_traits<_Alloc>::template rebind_alloc<_container_type>;

public:
  using value_type = T;
  using allocator_type = _Alloc;
  using size_type = typename std::allocator_traits<allocator_type>::size_type;
  using const_reference = const T&;

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

  constexpr size_type size() const
  {
    return _container->allocated;
  }

private:
  allocator_type _alloc;
  _container_type* _container;

  [[nodiscard]] _container_type* _construct()
  {
    static_assert(_Dimension == 2, "It's impossible to create matrix with a dimension greater than 2");
    _rebind_alloc ralloc(_alloc);
    _container_type* mtx = std::allocator_traits<_rebind_alloc>::allocate(ralloc, 1);
    std::allocator_traits<_rebind_alloc>::construct(ralloc, mtx);
    return mtx;
  }

  void _destruct(_container_type* mtx)
  {
    _rebind_alloc ralloc(_alloc);
    std::allocator_traits<_rebind_alloc>::destroy(ralloc, mtx);
    std::allocator_traits<_rebind_alloc>::deallocate(ralloc, mtx, 1);
  }
};

template<typename T, T _Value, class _Alloc>
class matrix_t<T, _Value, _Alloc, 1>
{
private:
  using _container_type = matrix_container_t<T>;
  using _self_matrix = matrix_t<T, _Value, _Alloc, 1>;

public:
  using value_type = T;
  using const_reference = const T&;
  using allocator_type = _Alloc;
  using size_type = typename std::allocator_traits<allocator_type>::size_type;

  ~matrix_t<T, _Value, allocator_type, 1>() = default;

  const_reference operator[](size_type idx) const
  {
    std::cout << "{const_reference 1}";
    return _Container->data[_OwnerDimension][idx];
  }
  stub_t operator[](std::size_t idx)
  {
    std::cout << "{reference to stub 1}";
    return stub_t(_Container->data[_OwnerDimension][idx]);
  }

  [[nodiscard]] constexpr size_type size() const noexcept
  {
    return _Container->allocated;
  }

private:
  _container_type * _Container;
  size_type _OwnerDimension;

  // доступ к конструктору с параметром из matrix_t с _Dimension==2
  friend matrix_t<T, _Value, allocator_type, 2>;
  matrix_t<T, _Value, allocator_type, 1>(_container_type* self, size_type N) : _Container(self), _OwnerDimension(N) { }

  // запрещаю напрямую создавать матрицу с измерением _Dimension=1
  matrix_t<T, _Value, allocator_type, 1>() = delete;
  const matrix_t<T, _Value, allocator_type, 1>& operator=(const matrix_t<T, _Value, allocator_type, 1>&) = delete;
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
  auto a = matrix[0]; std::cout << std::endl; // [0]
  foo(a);
  auto b = a[0]; std::cout << std::endl;
  foo(b);
  std::cout << "matrix[0][0] == " << matrix[0][0] << std::endl;
  std::cout << "matrix.size() == " << matrix.size() << std::endl;

  matrix[2][2] = 314;

  return 0;
}

// Сделать "типовую реализацию" двухмерной матрицы с proxy-классом, в котором были бы переопределены operator[]
// которая гуглится за 1 минуту,... было скучно. Поэтому пошёл своим путём, где контейнер matrix_t являет собой
// шаблон с шаблонными методами, имеющими циклическую реализацию. Так было интересно поупражняться в
// метапрограммировании.

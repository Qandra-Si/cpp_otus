// -*- mode: c++; coding: utf-8 -*-

#include <iostream>
#include <cstddef>

// см. похожий пример тут https://stackoverflow.com/a/1051468
struct stub_t
{
    explicit stub_t(int value) : value(value) { std::cout << "<direct> " << std::endl; }
    void operator=(const int& other) { std::cout << "<assign> " << std::endl; value = other; }
    operator int() { std::cout << "<copy> " << std::endl; return value; }

    // тут можно, конечно, ещё реализовать сравнение, но это не освободит от необходимости реализовать operator int()
    //bool operator==(const int& other) { std::cout << "<compare> " << std::endl; return value == other; }
private:
    int value;    
};

template<typename T>
struct foo_t
{
    using const_reference = const T&;
    T a[2] = {33, 42};
    // напрямую (сразу) возвращаем значение, без использования stub_t, оно всё равно не подвергнется изменениям
    const_reference operator[](std::size_t idx) const { std::cout << "[const_reference]" << std::endl; return a[idx]; }
    stub_t operator[](std::size_t idx) { std::cout << "[reference to stub]" << std::endl; return stub_t(a[idx]); }
};

int main()
{
    auto i = stub_t(33); // <direct>
    if (i == -1) { return -2; } // <copy>
    i = 42; // <assign>
    std::cout << i << std::endl; // <copy>

    const foo_t<int> s1;
    auto a1 = s1[0]; // [const_reference]

    foo_t<int> s2;
    auto a2 = s2[0]; // [reference to stub] <direct>
    if (a2 == -1) { return -1; } // <copy>
    s2[1] = 12; // [reference to stub] <direct> <assign>

    return a1;
}

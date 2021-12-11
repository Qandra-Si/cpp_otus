// -*- mode: c++; coding: utf-8 -*-
#include <array>
#include <stack>
#include <cstdint>
#include <cstdio> // In C++, it is recommended to use stdio instead of iostream to save a reasonable amount of memory.
#include <cstdlib>


/*! \brief Задача №1220. Stacks
*
* https://acm.timus.ru/problem.aspx?space=1&num=1220
*
* Imagine, that you are employed by a software development company. You work
* now on the famous "D++ project", which is devoted to the creation of a new
* generation programming language. Your particular task is quite prosaic, though.
* You are to develop the memory manager being able to work with a large number
* of stacks.
*
* Исходные данные: The first line of the input contains the total number of
* stack operations N, 0 < N ≤ 100000. Each of the next N lines contains a
* description of a stack operation, either in the form PUSH A B (meaning to push
* B into stack A), or in the form POP A (meaning to pop an element from stack A),
* where A is the number of stack (1 ≤ A ≤ 1000), and B is an integer
* (0 ≤ B ≤ 10^9). You may assume, that every operation is correct (i.e.,
* before each POP operation, the respective stack is not empty).
*
* Результат: For each POP operation, described in the input, output the value,
* which this POP operation gets from the top of that stack, to which it is
* applied. Numbers should appear according to the order of the POP operations
* in the input. Each number should be output in a separate line.
*
* In C++, it is recommended to use stdio instead of iostream to save a reasonable
* amount of memory.
*/
int main()
{
  int n, a, b;
  char cmd[5];

  // (0.75 * 1024 * 1024 / 100000) < 8 октет на одно входное число (мало для использования stl-объектов)
  if (scanf("%d", &n) == EOF) return 0;

  using stack_t = std::stack<int>;
  using stacks_t = std::array<stack_t*, 1000>;
  stacks_t stacks{0};

  // In C++, it is recommended to use stdio instead of iostream to save a reasonable amount of memory.
  while ((scanf("%s%d%d", cmd, &a, &b) >= 2) && n--)
  {
    stack_t **pstack = &stacks[a-1];
    // PUSH
    if (cmd[1] == 'U')
    {
      if (*pstack == nullptr)
        *pstack = new stack_t();
      (*pstack)->push(b);
    }
    // POP
    else if (cmd[1] == 'O')
    {
      printf("%d\n", (int)(*pstack)->top());
      (*pstack)->pop();
      if ((*pstack)->empty())
      {
        delete *pstack;
        *pstack = nullptr;
      }
    }
  }
  return 0;
}

// -*- mode: c++; coding: utf-8 -*-
#include <iostream>
#include <string>


/*! \brief Приращения (смещения) шахматной фигуры на доске
*/
struct movement_t
{
  int x_diff; /*!< приращение по горизонтали */
  int y_diff; /*!< приращение по вертикали */
  movement_t(int x_diff, int y_diff) : x_diff(x_diff), y_diff(y_diff) { }
};

/*! \brief Позиция шахматной фигуры на доске
*/
struct position_t
{
  int x; /*!< позиция по горизонтали */
  int y; /*!< позиция по вертикали */
  position_t(const std::string & s) : x(s[0]-'a'), y(s[1]-'1') { }
  position_t operator+(const movement_t & mov) const
  {
    position_t res(*this);
    res.x += mov.x_diff;
    res.y += mov.y_diff;
    return res;
  }
};

/*! \brief Шахматный конь, ходит буквой "Г"
*/
struct chess_knight_t
{
  position_t pos;
  chess_knight_t(const std::string & s) : pos(s) { }
  int get_valid_moves_count() const;
};

std::string trim(const std::string & s);

/*! \brief Задача №1197. Один в поле воин
*
* https://acm.timus.ru/problem.aspx?space=1&num=1197
*
* Условие этой задачи очень простое: вам всего лишь надо определить, сколько
* клеток находится под боем шахматного коня, одиноко стоящего на шахматной
* доске. На всякий случай напомним, что конь ходит буквой "Г" — на две клетки по
* горизонтали или вертикали в любом направлении, и потом на одну клетку в
* направлении, перпендикулярном первоначальному.
*
* Исходные данные: В первой строке находится единственное число N,
* 1 <= N <= 64 — количество тестов. В каждой из последующих N строк содержится
* очередной тест: два символа (маленькая латинская буква от 'a' до 'h' и цифра
* от 1 до 8) — стандартное шахматное обозначение клетки, на которой стоит конь.
* При этом буква обозначает вертикаль, а цифра — горизонталь.
*
* Результат: Выведите N строк: в каждой из них должно находиться единственное
* число — количество клеток шахматной доски, находящихся под боем коня.
*/
int main()
{
  int n;
  std::cin >> n;
  std::string line;
  while (std::getline(std::cin, line))
  {
    line = trim(line);
    if (line.empty()) continue; // первая строка прочиталась до конца?
    chess_knight_t knight(line);
    std::cout << knight.get_valid_moves_count() << std::endl;
    if (!(--n)) break;
  }
  return 0;
}

// к сожалению, getline дочитывает до конца строки после полученного int, т.ч.
// если по следующей позиции будут найдены whitespaces, то они попадут на вход
// как пустая или заполненная пробелами строка, - удаляем их
std::string trim(const std::string & s)
{
  const static std::string WHITESPACES = " \n\r\t\f\v";
  const std::size_t from = s.find_first_not_of(WHITESPACES);
  if (from == std::string::npos) return "";
  std::string res = s.substr(from);
  const std::size_t to = res.find_last_not_of(WHITESPACES);
  if (to == std::string::npos) return "";
  return res.substr(0, to + 1);
}

const static movement_t chess_knight_moves[8] = {
 movement_t( -2, -1 ),
 movement_t( -2,  1 ),
 movement_t( -1, -2 ),
 movement_t(  1, -2 ),
 movement_t(  2,  1 ),
 movement_t(  2, -1 ),
 movement_t(  1,  2 ),
 movement_t( -1,  2 ),
};

int chess_knight_t::get_valid_moves_count() const
{
  int res = 0;
  for (int i = 0; i < sizeof(chess_knight_moves) / sizeof(chess_knight_moves[0]); ++i)
  {
    position_t finish = this->pos + chess_knight_moves[i];
    if (finish.x >= 0 && finish.x < 8 && finish.y >= 0 && finish.y < 8) res++;
  }
  return res;
}

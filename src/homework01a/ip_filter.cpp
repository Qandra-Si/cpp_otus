// -*- mode: c++; coding: utf-8 -*-

// От работы со строковым представлением IPv4 адреса было решено отказаться сразу,
// в противовес с работой с числами. Строковое представление адреса парсится только
// на входе и выводится как результат. Во первых: это удобнее, во вторых: быстрее.
// Более того, октеты адреса располагаются в той же памяти, что и uint32_t число
// (велосипед не изобретаем, ядро линукса устроено так же). Но при использовании
// операций сравнения над этими uint32_t значениями, мы можем столкнуться с проблемой
// когда порядок октет в числе либо big-endian, либо little-endian, так что надо
// уметь детектировать порядок октет в числе на целевой платформе, и иметь две
// реализации сортировок. Далее тривиально - все условия вывода записываются as is и
// несложным образом проверяем ячейки памяти, где располагаются октеты IPv4 адресов.

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <bit>
#include <ranges>


std::vector<std::string> split(const std::string & s, char delimiter)
{
  std::string item;
  std::vector<std::string> items;
  std::istringstream iss(s);
  while(std::getline(iss, item, delimiter))
  {
    items.push_back(item);
  }
  return items;
}

union ipv4_t
{
  uint8_t v_addr[4];
  uint32_t numeric;
};

bool split_ipv4(const std::string & s, ipv4_t & ipv4)
{
  uint8_t * v_cursor = ipv4.v_addr;
  const uint8_t * v_end = v_cursor + sizeof(ipv4.v_addr);
  std::istringstream iss(s);
  std::string snum;
  while(std::getline(iss, snum, '.'))
  {
    if (v_cursor == v_end) return false;
    *v_cursor++ = (uint8_t)std::stoul(snum);
  }
  return v_cursor == v_end;
}

static int big_endian_cmp(const void *x, const void *y)
{
  const ipv4_t * _x = static_cast<const ipv4_t*>(x);
  const ipv4_t * _y = static_cast<const ipv4_t*>(y);
  if (_x->numeric < _y->numeric) return 1;
  if (_x->numeric > _y->numeric) return -1;
  return 0;
}

#define be32(n32) ((n32>>24)&0xff) |    \
                  ((n32<<8)&0xff0000) | \
                  ((n32>>8)&0xff00) |   \
                  ((n32<<24)&0xff000000)

static int little_endian_cmp(const void *x, const void *y)
{
  // C++23 будет поддерживать метод реверса байт в числе, а пока используем макрос по старинке
  uint32_t _x = static_cast<const ipv4_t*>(x)->numeric;
  uint32_t _y = static_cast<const ipv4_t*>(y)->numeric;
  // в принципе, продетектировав платформу, можно было бы сразу октеты грузить в структуру ipv4_t
  // в обратном порядке, это бы значительно снизило расходы во время сортировки, он добавило бы
  // путаницы при выводе октет адреса в строковом представлении... не будем усложнять, не сервер пишем
  _x = be32(_x);
  _y = be32(_y);
  if (_x < _y) return 1;
  if (_x > _y) return -1;
  return 0;
}

void print_ipv4(std::ostream& output, const ipv4_t & ipv4)
{
  output
    << (int)ipv4.v_addr[0] << '.'
    << (int)ipv4.v_addr[1] << '.'
    << (int)ipv4.v_addr[2] << '.'
    << (int)ipv4.v_addr[3] << std::endl;
}

template <typename Pred>
void filter_ip_pool(std::ostream& output, const std::vector<ipv4_t> & ip_pool, Pred pred)
{
  // C++20 поддерживает работу с предикатами фильтрации в ranges,
  // пользуемся pipe для передачи элементов вектора в фильтрацию
  for (const auto & ipv4 : ip_pool | std::views::filter(pred))
  {
    print_ipv4(output, ipv4);
  }
}

int main(int, char* [])
{
  ipv4_t ipv4;
  std::string line;
  std::vector<std::string> line_lexems;
  std::vector<ipv4_t> ip_pool;
  try
  {
    // со стандартного ввода снимаем входной поток лексем, разделённых табуляцией
    // нас интересует только первый фрагмент, который является ipv4 адресом, остальные
    // фрагменты отбрасываем
    // все ipv4-фрагменты во время загрузки преобразуются в числа, которые больше
    // подходят для решения задач, поставленных во 2м упражнении 1го д.з.
    while (std::getline(std::cin, line))
    {
      line_lexems = split(line, '\t');
      if (line_lexems.empty()) break; // разве этот ввод так может закончиться?
      if (!split_ipv4(line_lexems.front(), ipv4))
      {
        // работа с неправильными ipv4 адресами не определена условием задачи
        // впрочем, сделать вид, что всё в порядке тоже не будем
        throw std::invalid_argument("Illegal IPv4 address format: " + line_lexems.front());
      }
      ip_pool.push_back(ipv4);
    }

    // определяем порядок байт в числе (little-endian или big-endian?)
    // C++20 уже поддерживает нативное определение порядка байт в числе
    std::qsort(
      ip_pool.data(), ip_pool.size(), sizeof(ipv4_t),
      // здесь нельзя 'напрямую' сравнивать uint32_t представление ipv4, т.к. на платформах
      // с big-endian представлением чисел, данные отсортируются в обратном порядке
      (std::endian::native == std::endian::little) ? little_endian_cmp : big_endian_cmp
    );

    // выводим адреса из пула в отсортированном виде (без фильтрации)
    filter_ip_pool(
        std::cout,
        ip_pool,
        [](const ipv4_t&) { return true; }
    );
    // выводим адреса из пула, первый октет которых равен 1
    filter_ip_pool(
        std::cout,
        ip_pool,
        [](const ipv4_t& ipv4) { return ipv4.v_addr[0] == 1; }
    );
    // выводим адреса из пула, первый октет которых равен 46, а второй 70
    filter_ip_pool(
        std::cout,
        ip_pool,
        [](const ipv4_t& ipv4) { return (ipv4.v_addr[0] == 46) && (ipv4.v_addr[1] == 70); }
    );
    // выводим адреса из пула, любой октет которых равен 46
    filter_ip_pool(
        std::cout,
        ip_pool,
        [](const ipv4_t& ipv4) { return (ipv4.v_addr[0] == 46) || (ipv4.v_addr[1] == 46) || (ipv4.v_addr[2] == 46) || (ipv4.v_addr[3] == 46); }
    );
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  catch (...)
  {
    std::cerr << "Unknown exception while processing input data" << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

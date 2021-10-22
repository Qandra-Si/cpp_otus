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

#include <stdexcept>
#include <iostream>
#include <cstdlib>

#include <core/utils.h>
#include <core/ipv4.h>


void print_ipv4(std::ostream& output, const core::ipv4_t & ipv4)
{
  output
    << (int)ipv4.v_addr[0] << '.'
    << (int)ipv4.v_addr[1] << '.'
    << (int)ipv4.v_addr[2] << '.'
    << (int)ipv4.v_addr[3] << std::endl;
}

template <typename Pred>
void filter_ip_pool(std::ostream& output, const std::vector<core::ipv4_t>& ip_pool, Pred pred)
{
    // C++20 поддерживает работу с предикатами фильтрации в ranges,
    // здесь можно было бы воспользовать pipe для передачи элементов вектора в фильтрацию
    // for (const auto& ipv4 : ip_pool | std::views::filter(pred))
    for (const auto& ipv4 : ip_pool)
    {
        if (!pred(ipv4)) continue;
        print_ipv4(output, ipv4);
    }
}

int main(int, char* [])
{
  core::ipv4_t ipv4;
  std::string line;
  std::vector<std::string> line_lexems;
  core::ip_pool_t ip_pool;

  line_lexems.reserve(3);
  ip_pool.reserve(1000);

  try
  {
    // со стандартного ввода снимаем входной поток лексем, разделённых табуляцией
    // нас интересует только первый фрагмент, который является ipv4 адресом, остальные
    // фрагменты отбрасываем
    // все ipv4-фрагменты во время загрузки преобразуются в числа, которые больше
    // подходят для решения задач, поставленных во 2м упражнении 1го д.з.
    while (std::getline(std::cin, line))
    {
      core::split_into(line, '\t', line_lexems);
      if (line_lexems.empty()) break; // разве этот ввод так может закончиться?
      if (!core::split_ipv4(line_lexems.front(), ipv4))
      {
        // работа с неправильными ipv4 адресами не определена условием задачи
        // впрочем, сделать вид, что всё в порядке тоже не будем
        throw std::invalid_argument("Illegal IPv4 address format: " + line_lexems.front());
      }
      ip_pool.push_back(ipv4);
    }

    // в принципе, продетектировав платформу, можно было бы сразу октеты грузить в структуру ipv4_t
    // в обратном порядке, это бы значительно снизило расходы во время сортировки, но добавило бы
    // путаницы при выводе октет адреса в строковом представлении... не будем усложнять, не сервер пишем

    // определяем порядок байт в числе (little-endian или big-endian?)
    // C++20 уже поддерживает нативное определение порядка байт в числе
    sort_ipv4_pool(
      ip_pool,
      // здесь нельзя 'напрямую' сравнивать uint32_t представление ipv4, т.к. на платформах
      // с big-endian представлением чисел, данные отсортируются в обратном порядке
        core::is_little_endian() ? core::little_endian_ipv4_cmp_reverse : core::big_endian_ipv4_cmp_reverse
    );

    // выводим адреса из пула в отсортированном виде (без фильтрации)
    filter_ip_pool(
        std::cout,
        ip_pool,
        [](const core::ipv4_t&) { return true; }
    );
    // выводим адреса из пула, первый октет которых равен 1
    filter_ip_pool(
        std::cout,
        ip_pool,
        [](const core::ipv4_t& ipv4) { return ipv4.v_addr[0] == 1; }
    );
    // выводим адреса из пула, первый октет которых равен 46, а второй 70
    filter_ip_pool(
        std::cout,
        ip_pool,
        [](const core::ipv4_t& ipv4) { return (ipv4.v_addr[0] == 46) && (ipv4.v_addr[1] == 70); }
    );
    // выводим адреса из пула, любой октет которых равен 46
    filter_ip_pool(
        std::cout,
        ip_pool,
        [](const core::ipv4_t& ipv4) { return (ipv4.v_addr[0] == 46) || (ipv4.v_addr[1] == 46) || (ipv4.v_addr[2] == 46) || (ipv4.v_addr[3] == 46); }
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

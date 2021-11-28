// -*- mode: c++; coding: utf-8 -*-
#include <iostream>
#include <vector>
#include <list>
#include <set>


typedef std::vector<std::vector<int>> graph_t;
typedef std::set<int> visited_t;
typedef std::list<visited_t> graph_components_t;
typedef std::vector<std::pair<int,int>> links_t;

void load_graph(int num_nodes, int num_pairs, graph_t& graph);
void graph_connectivity(int num_nodes, const graph_t& graph, graph_components_t & isolated);
int gray_code(int n);
int count_bits(int n);

/*! \brief Задача №1272. Метро не в Екатеринбурге
*
* https://acm.timus.ru/problem.aspx?space=1&num=1272
*
* В небольшом городке началось строительство метрополитена. Особенностью городка
* является то, что он стоит на маленьких островах, некоторые из которых соединены
* тоннелями или мостами. По убеждению мэра, метро должно быть проложено под землей,
* поэтому в проекте строительства должно использоваться как можно меньше мостов.
* Единственное требование, которое предъявляется к метрополитену, заключается в
* том, чтобы жители города могли добраться на метро (возможно, с пересадками) с
* любого острова на любой другой. К счастью, известно, что мостов и тоннелей для
* этого достаточно. Из соображений экономии было решено построить как можно меньше
* переездов между островами.
*
* Ваша задача: зная план города, определить минимально возможное количество мостов,
* которые необходимо задействовать при строительстве метрополитена.
*
* Исходные данные: В первой строке через пробел записаны три целых числа N
* (количество островов, 1 <= N <= 10000), K (количество тоннелей, 0 <= K <= 12000)
* и M (количество мостов, 0 <= M <= 12000). Затем следуют K строк, в каждой из
* которых записаны два числа - номера островов, соединённых соответствующим
* тоннелем. Последние M строк описывают мосты в аналогичном формате.
*
* Результат: должен содержать единственное число - минимальное число мостов, которые
* необходимо задействовать при строительстве метрополитена.
*/
int main()
{
  int num_islands, num_tunnels, num_bridges;
  std::cin >> num_islands >> num_tunnels >> num_bridges;

  // сразу проверяем исключительные ситуации, чтобы не делать лишнего
  if (num_islands == 1)
  {
    std::cout << 0 << std::endl; // мосты не нужны
    return 0;
  }

  // загружаем систему туннелей
  graph_t tunnels;
  graph_components_t tunnel_zones;
  if (num_tunnels)
  {
    load_graph(num_islands, num_tunnels, tunnels);
    // выясняем сегменты в системе туннелей
    graph_connectivity(num_islands, tunnels, tunnel_zones);
    if (tunnel_zones.size() == 1 && tunnel_zones.front().size() == num_islands)
    {
      // туннелей в метро достаточно, они связывают все острова
      std::cout << 0 << std::endl;
      return 0;
    }
    else if (num_bridges == 0)
    {
      // вообще-то это ошибка в данных, т.к. по условию задачи граф полностью связан, но...
      std::cout << tunnel_zones.size() - 1 << std::endl;
      return 0;
    }
  }

  // загружаем систему мостов
  links_t bridges;
  if (num_bridges)
  {
    bridges.reserve(num_bridges);
    for (int i = 0; i < num_bridges; ++i)
    {
      int i1, i2;
      std::cin >> i1 >> i2;
      bridges.push_back(links_t::value_type(i1,i2));
    }
  }

  // итак, в данном месте мы имеем несколько несвязанных между собой кусков
  // метрополитена, между которыми (по условию задачи) проложены мосты, теперь
  // надо удалить лишние мосты, оставив минимально-необходимое кол-во
  
  // находим те мосты, которые не влияют на связность метрополитена и сразу удаляем их
  for (links_t::iterator itr = bridges.begin(), end = bridges.end(); itr != end; )
  {
    const links_t::value_type& link = *itr;
    bool found = false;
    for (const auto& zone : tunnel_zones)
      if (zone.find(link.first) != zone.end() && zone.find(link.second) != zone.end())
      {
        found = true;
        break;
      }
    if (!found)
      ++itr;
    else
    {
      itr = bridges.erase(itr);
      end = bridges.end();
    }
  }

  int min_num_required_bridges = tunnel_zones.size() - 1;

  // перебор всех возможных комбинаций мостов
  // начинаем подмешивать к графу туннелей комбинации мостов и ждать, когда граф станет
  // полностью связным, ... повторяем, пока не будет подмешан лишь один мост
  num_bridges = bridges.size();
  if (num_bridges)
  {
    graph_t variant_of_tunnels;
    graph_components_t variant_of_tunnel_zones;
    for (int k = 1, n = num_bridges; k <= n; k++)
    {
      for (int i = 0; i < (1 << n); ++i)
      {
        int cur = gray_code(i);
        if (count_bits(cur) == k)
        {
          // сочетания из N элементов (мостов) по K (штук) в лексикографическом порядке
          variant_of_tunnels = tunnels;
          for (int j = 0; j < n; ++j)
            if (cur & (1 << j))
            {
              const auto& bridge = bridges[j];
              // меняем систему туннелей, добавляя в неё мосты
              variant_of_tunnels[bridge.first-1].push_back(bridge.second);
              variant_of_tunnels[bridge.second-1].push_back(bridge.first);
            }
          // выясняем сегменты в системе туннелей + добавленные мосты
          graph_connectivity(num_islands, variant_of_tunnels, variant_of_tunnel_zones);
          if (variant_of_tunnel_zones.size() == 1 && variant_of_tunnel_zones.front().size() == num_islands)
          {
            // связей между островами достаточно, а поскольку сочетания перебирались от
            // минимального кол-ва к максимальному кол-ву, то k - это ответ
            std::cout << k << std::endl;
            return 0;
          }
        }
      }
    }
  }

  std::cout << min_num_required_bridges << std::endl;
  return 0;
}

void load_graph(int num_nodes, int num_pairs, graph_t& graph)
{
  graph.resize(num_nodes);
  for (int i = 0; i < num_pairs; ++i)
  {
    int i1, i2;
    std::cin >> i1 >> i2;
    //if (graph[i1-1].empty()) graph[i1-1].reserve(num_nodes);
    graph[i1-1].push_back(i2);
    //if (graph[i2-1].empty()) graph[i2-1].reserve(num_nodes);
    graph[i2-1].push_back(i1);
  }
}

void dfs(const graph_t& graph, visited_t& visited, int v)
{
  visited.insert(v);
  const graph_t::value_type& node = graph[v-1];
  for (graph_t::value_type::const_iterator itr = node.begin(), end = node.end(); itr != end; ++itr)
  {
    const int & link = *itr;
    if (visited.find(link) == visited.end())
      dfs(graph, visited, link);
  }
}

void graph_connectivity(int num_nodes, const graph_t& graph, graph_components_t& isolated)
{
  isolated.clear();

  visited_t visited;
  for (int node = 1; node <= num_nodes; ++node)
  {
    // пропускаем вершины графа, которые уже алгоритм обходил в каком либо компоненте
    if (visited.find(node) != visited.end()) continue;
    // строим подграф-компонент изолированный от других вершин графа
    visited_t component;
    dfs(graph, component, node);
    visited.insert(component.begin(), component.end());
    // сохраняем компонент в список изолированных подграфов
    //if (component.size() > 1)
    {
      isolated.push_back(component);
    }
  }
}

int gray_code(int n)
{
  return n ^ (n >> 1);
}

int count_bits(int n)
{
  int res = 0;
  for (; n; n >>= 1)
    res += n & 1;
  return res;
}

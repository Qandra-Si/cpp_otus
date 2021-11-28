// -*- mode: c++; coding: utf-8 -*-
#include <iostream>
#include <vector>
#include <list>
#include <set>


typedef std::vector<std::vector<int>> graph_t;
typedef std::set<int> visited_t;
typedef std::list<visited_t> graph_components_t;

void load_graph(int num_nodes, int num_pairs, graph_t& graph);
void graph_connectivity(int num_nodes, const graph_t& graph, graph_components_t & isolated);

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
  else if (num_tunnels == 0)
  {
    std::cout << num_islands - 1 << std::endl; // метро нет, мостов надо : острова - 1
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
  }

  int min_num_required_bridges = tunnel_zones.size() - 1;

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

// -*- mode: c++; coding: utf-8 -*-

#include <list>
#include <mutex>

#include "threadsafe_writer.h"


namespace homework25
{

//------------------------------------------------------------------------------
// my_command_t
//------------------------------------------------------------------------------
void my_command_t::handle()
{
}

//------------------------------------------------------------------------------
// my_set_of_commands_t::impl_t
//------------------------------------------------------------------------------
class my_set_of_commands_t::impl_t
{
public:
  volatile bool& m_terminated;
  std::mutex m_commands_guard;
  std::list<my_command_t*> m_commands;

  impl_t(volatile bool& terminated) : m_terminated(terminated)
  {
  }
};

//------------------------------------------------------------------------------
// my_set_of_commands_t
//------------------------------------------------------------------------------
my_set_of_commands_t::my_set_of_commands_t(volatile bool& terminated) : p_impl(new impl_t(terminated))
{
}

my_set_of_commands_t::~my_set_of_commands_t()
{
  delete p_impl;
}

bool my_set_of_commands_t::wait_for_handle()
{
  while (1)
  {
    if (p_impl->m_terminated) return false;
    if (pop_handle()) break;
    std::this_thread::yield();
  }
  return true;
}

void my_set_of_commands_t::push_command(my_command_t* command)
{
  std::scoped_lock<std::mutex> locker(p_impl->m_commands_guard);
  p_impl->m_commands.emplace_back(command);
}

bool my_set_of_commands_t::pop_handle()
{
  p_impl->m_commands_guard.lock();
  if (p_impl->m_commands.empty()) { p_impl->m_commands_guard.unlock(); return false; }
  // в данной точке мы уже знаем, что к нам поступила новая команда, которую надо
  // обработать; с одной стороны мы можем обрабатывать все команды в хронологическом
  // порядке, а с другой... какая разница в каком порядке они будут обработаны?!
  // ---
  // например, обработка последней поступившей команды наиболее выгодна, поскольку
  // онf вероятнее всего находится в кеше процессора, так что LIFO может дать ощутимый
  // прирост в скорости обработки очереди команд
  // ---
  // всё же, пусть будет хронологический FIFO порядок обработки
  auto command = p_impl->m_commands.front();
  p_impl->m_commands.pop_front();
  p_impl->m_commands_guard.unlock();

  // это очень важная точка в программе (пожалуй самая важная!)
  // --
  // в этой точке текущий <leader> поток сменит роль на <processor> поток, а следующий
  // ожидающий <follower> займёт место <leader> потока, получит доступ к <handle set>
  // и сможет получить новую команду, начав её обработку
  command->handle_event();
  return true;
}

}
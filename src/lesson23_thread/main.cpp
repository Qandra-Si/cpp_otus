#include <thread>
#include <chrono>
#include <string>

struct runner_unstable_t
{
  std::thread runner;
  volatile bool terminated;

  runner_unstable_t() : terminated(false)
  {
    // запускаем поток (метод run получит инициализацию и выдет выполняться пока не будет вызван деструктор)
    std::string tmp = u8"Hello, World! (какой-то набор параметров, занимающий приличное место на стеке)";
    runner = std::thread(&runner_unstable_t::run, this, tmp);
  }

  ~runner_unstable_t()
  {
    // завершаем работу потока
    terminated = true;
    if (runner.joinable())
    {
      if (runner.get_id() == std::this_thread::get_id())
        runner.detach();
      else
        runner.join();
    }
  }

  void run(std::string some_parameter)
  {
    // проверяем значение пераметра (просто, чтобы оптимизиатор не выкинул его)
    // в этой реализации значение парамтера some_parameter нестабильно
    if (some_parameter.empty()) return;
    // крутимся в вечном цикле, пока нас не остановили
    while (!terminated)
    {
      std::this_thread::yield();
    }
  }
};


int main()
{
  {
    runner_unstable_t runner;

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(5000ms);

    runner.terminated = true;
  }

  return 0;
}

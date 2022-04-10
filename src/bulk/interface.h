// -*- mode: c++; coding: utf-8 -*-
/*! \file interface.h
* \brief Интерфейсная часть домашнего здаания 15ой лекции
*/

#pragma once

#include <string>
#include <list>
#include <memory>
#include <ctime>
#include <functional>



/*! \defgroup homework15 Интерфейсные классы программы, выполняющей команды
*
* Подробное описание назначения классов см. в корневом README.md файле.
*/

/*! \addtogroup homework15
* @{
*/

namespace homework15 {

/*! \brief Интерфейс команды, которую можно выполнить
*
* Можно было бы сделать так:
\code
virtual void execute() = 0;
\endcode
* Но именно такой подход с методом \ref execute, который при выполнении
* команды делает что-то кроме вывода текста на экран... в данном приложении и
* проекте был был неудачным. Дело в том, что по условию задачи команда "не знает"
* в каких условиях она будет выполнена и куда будет направлен результат её
* деятельности (контекста нет). Например, команда должна была бы быть выполнена
* дважды - один раз вывести информацию в терминал, второй раз в файл которого
* "пока нет", и это выглядит слишком криво. Можно было-бы "накрутить логику" и
* передать команде несколько контекстов - пусть результаты сама доставляет, но
* такая реализация (если делать не через одно место) сравнялась бы по сложности
* и количеству кода со всем домашним заданием вместе взятым.
*
* Поэтому пришлось сделать так:
\code
virtual std::string execute() = 0;
\endcode
* Хотя теперь такую реализацию можно назвать избыточной. Класс \ref abstract_command_t
* уже не нужен, кроме как для сохранения строки с текстом команды... Ну и пусть,
* зато \ref commands_processor_t сможет выполнить команду один раз, результат её
* выполнения сохранить и направлять в терминал и в файл самостоятельно. Бинго!
* (хотя тоже не очень красиво)
*/
class abstract_command_t
{
public:
  virtual std::string execute() = 0;
};

/*! \brief Интерфейс планировщика команд, который позволяет подготавливать их выполнение
*/
class abstract_bulk_t
{
public:
  virtual void prepare(abstract_command_t* cmd) = 0;
  virtual unsigned start_transaction() = 0;
  virtual unsigned commit_transaction() = 0;
};

/*! \brief Кастомная команда
*
* Простейшая реализация интерфейса \ref abstract_command_t, которая сохраняет
* строку (сама является строкой) и выдаёт её при вызове метода \ref execute.
*/
class custom_command_t : public abstract_command_t, private std::string
{
public:
  custom_command_t(const std::string& cmd);
  virtual ~custom_command_t() = default;
public: // реализация интерфейса abstract_command_t
  virtual std::string execute() override;
private: // отключаем конструктор и оператор копирования
  custom_command_t() = delete;
  custom_command_t(const custom_command_t&) = delete;
  const custom_command_t& operator=(const custom_command_t&) = delete;
};

/*! \brief Набор задержанных к выполнению команд
*
* Хранит команды до тех пор, пока не наступит момент их выполнить. Засекает
* время при добавлении первой команды. Имеет метод \ref run, с помощью которого
* накопленные команды выполняются. Результат выполнения команд отправляет к
* терминал, а также сохраняет в архивный лог.
*/
class commands_processor_t : private std::list<std::unique_ptr<custom_command_t>>
{
public:
  using finalizer_t = std::function<void(const std::time_t& t, const std::string& line)>;
  commands_processor_t(const finalizer_t& finalizer) : finalizer(finalizer) { }
  void add(abstract_command_t* const cmd);
  void run();
  size_type bulk_size() const { return size(); }
private: // отключаем конструктор и оператор копирования
  commands_processor_t() = delete;
  commands_processor_t(const commands_processor_t&) = delete;
  const commands_processor_t& operator=(const commands_processor_t&) = delete;

  std::time_t t{ 0 }; /*!< время получения первой команды в группе */
  finalizer_t finalizer; /*!< завершение сборки набора команд и выполнение действий с ними */
};

/*! \brief Кастомный планировщик команд
*
* Имеет реализацию интерфейса \ref abstract_bulk_t. Считает сохранённые
* (отложенные) для выполнения команды и когда их накапливается заданное
* количество - обращается к методу run экземпляра класса \ref commands_processor_t.
* Отслеживает групповые операции (транзакции) с помощью которых подготавлиется
* единая группа команд до тех пор, пока не будут закрыты все вложенные
* транзакции, - почле чего также вызывает метод run обработчика команд.
*/
class custom_bulk_t : public abstract_bulk_t
{
public:
  custom_bulk_t(unsigned n, const commands_processor_t::finalizer_t& finalizer);
  virtual ~custom_bulk_t();
public: // реализация интерфейса abstract_bulk_t
  virtual void prepare(abstract_command_t* const cmd) override;
  virtual unsigned start_transaction() override;
  virtual unsigned commit_transaction() override;
private:
  const unsigned bulk_size; /*!< кол-во команд в одном bulk, которые выполняются совместно */
  unsigned num_nested_transactions; /*!< кол-во вложенных транзакций */
  commands_processor_t delayed_commands; /*!< список уникальных указателей на отложенные команды */
private: // отключаем конструктор и оператор копирования
  custom_bulk_t() = delete;
  custom_bulk_t(const custom_bulk_t&) = delete;
  const custom_bulk_t& operator=(const custom_bulk_t&) = delete;
};

}

/*! @} */

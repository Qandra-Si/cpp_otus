// -*- mode: c++; coding: utf-8 -*-
/*! \file logger.h
* \brief Логгер домашнего задания 25ой лекции
*/

#pragma once

#include <iostream>



/*! \defgroup homework25 Интерфейсные классы программы, выполняющей команды асинхронно
*
* Подробное описание назначения классов см. в корневом README.md файле.
*/

/*! \addtogroup homework25
* @{
*/

namespace homework25 {

/*! \brief Абстрактный интерфейс отправки сообщений в журнал
*/
class abstract_logger_t
{
public:
  virtual void log(const char * line) = 0;
};

/*! \brief Логгер сообщений, который работает в отдельном потоке
* 
* Логгер имеет custom-реализацию конструктора и деструктора, которые принимают
* std::ostream для вывода сообщений в терминал и "под капотом" запускают отдельный
* thread для асинхронного вывода накопившихся сообщений.
*
* Также логгер реализует интерфейс \ref abstract_logger_t для предоставления другим
* сущностям возможность отправки сообщений в журнал.
*/
class logger_t
{
public:
  logger_t(std::ostream& terminal);
  ~logger_t();
  abstract_logger_t* get_interface();

private:
  // скрытая внутреняя реализация (д.б. объявлен последним)
  class impl_t;
  impl_t* p_impl;
};

}

/*! @} */

// -*- mode: c++; coding: utf-8 -*-
/*! \file custom.h
* \brief Декларации "специфической" реализации классов домашнего задания 13ой лекции
*/

#pragma once

#include "sketch.h"

#include <list>


/*! \addtogroup homework13
* @{
*/

namespace homework13 {

/*! \brief Некий загадочный интерфейс импорта данных
*
* Умеет делать read числовых данных, а вот куда - непонятно.
*/
class import_interface_t
{
public:
  virtual void read(int&) const = 0;
  virtual void read(unsigned&) const = 0;
};

/*! \brief Некий загадочный интерфейс экспорта данных
*
* Умеет делать write числовых данных, а вот куда - непонятно.
*/
class export_interface_t
{
public:
  virtual void write(const int&) = 0;
  virtual void write(const unsigned&) = 0;
};

/*! \brief Некий загадочный файл с импортом и экспортом
*
* Реализация файла потребует выполнить импорт и экспорт числовых данных.
*/
class file_t : public import_interface_t, public export_interface_t
{
};

/*! \brief Кастомный графический примитив
*
* Внимание! данный интерфейс \ref custom_primitive_t не имеет ничего общего с
* примитивом \ref primitive_t. На данном этапе катомный интерфейс декларирует
* лишь метод \ref draw и предполагает, что с его помощью будет реализовано
* действие render во view-сущности, работающей с примитивами.
*
* - \see primitive_t
* - \see console_view_t
*/
struct custom_primitive_t
{
  virtual void draw() = 0;
};

/*! \brief Кастомный графический примитив "окружность"
*
* Реализует интерфейсы окружности \ref circle_t и кастомного примитива \ref custom_primitive_t.
*
* Умеет экспортироваться и импортироваться с использованием интерфейсов
* \ref export_interface_t и \ref import_interface_t. Также умеет отображаться
* в документе реализую рендеринг view-сущности. Является частью model-сущности.
*
* - \see custom_rectangle_t
* - \see custom_model_t
* - \see console_view_t
*/
class custom_circle_t : public circle_t, public custom_primitive_t
{
public:
  custom_circle_t(int x, int y, unsigned radius);
  virtual ~custom_circle_t() = default;
public: // реализация интерфейса primitive_t
  virtual void export_to_file(export_interface_t*) override;
  virtual void import_from_file(const import_interface_t*) override;
public: // реализация интерфейса custom_primitive_t
  virtual void draw() override;
private: // отключаем конструктор и оператор копирования (слишком простая реализация)
  custom_circle_t() = delete;
  custom_circle_t(const custom_circle_t&) = delete;
  const custom_circle_t& operator=(const custom_circle_t&) = delete;
};

/*! \brief Кастомный графический примитив "прямоугольник"
*
* Реализует интерфейсы окружности \ref circle_t и кастомного примитива \ref custom_primitive_t.
*
* Умеет экспортироваться и импортироваться с использованием интерфейсов
* \ref export_interface_t и \ref import_interface_t. Также умеет отображаться
* в документе реализую рендеринг view-сущности. Является частью model-сущности.
*
* - \see custom_circle_t
* - \see custom_model_t
* - \see console_view_t
*/
class custom_rectangle_t : public rectangle_t, public custom_primitive_t
{
public:
  custom_rectangle_t(int left, int top, unsigned width, unsigned height);
  virtual ~custom_rectangle_t() = default;
public: // реализация интерфейса primitive_t
  virtual void export_to_file(export_interface_t*) override;
  virtual void import_from_file(const import_interface_t*) override;
public: // реализация интерфейса custom_primitive_t
  virtual void draw() override;
private: // отключаем конструктор и оператор копирования (слишком простая реализация)
  custom_rectangle_t() = delete;
  custom_rectangle_t(const custom_rectangle_t&) = delete;
  const custom_rectangle_t& operator=(const custom_rectangle_t&) = delete;
};

/*! \brief Модель (model) с которой взаимодействует controller и которую использует view
*
* Хранит данные документа с графическими примитивами, предоставляет к ним
* доступ.
*
* В данной простейшей реализации сразу является кастомной моделью, поскольку с
* нею работают только кастомные классы.
*
* - \see controller_t
* - \see console_view_t
*/
class custom_model_t
{
public:
  custom_model_t() = default;
  virtual ~custom_model_t() = default;
  // у экземпляров объектов нет общего предка, используется множественное наследование
  using circles_t = std::list<std::unique_ptr<custom_circle_t>>;
  using rectangles_t = std::list<std::unique_ptr<custom_rectangle_t>>;
  circles_t& get_circles() { return circles; }
  rectangles_t& get_rectangles() { return rectangles; }
  const circles_t& get_circles() const { return circles; }
  const rectangles_t& get_rectangles() const { return rectangles; }
  // методы-утилиты сущности model
  custom_circle_t* add_circle(int x, int y, unsigned radius);
  custom_rectangle_t* add_rectangle(int left, int top, unsigned width, unsigned height);
  void move(primitive_t* const primitive, int offset_x, int offset_y);
  void remove(primitive_t* const primitive);
private:
  circles_t circles;
  rectangles_t rectangles;
private: // отключаем конструктор и оператор копирования (слишком простая реализация)
  custom_model_t(const custom_model_t&) = delete;
  const custom_model_t& operator=(const custom_rectangle_t&) = delete;
};

/*! \brief Представление (view) графических примитивов с помощью консольного вывода
*
* Выполняет "рендеринг" графических векторных примитивов непосредственно в
* текстовый терминал.
*
* В данной простейшей реализации сразу является кастомным представлением,
* поскольку с ним работают только кастомные классы.
*
* - \see controller_t
* - \see custom_model_t
*/
class console_view_t
{
public:
  console_view_t() = default;
  void attach_model(custom_model_t* model);
  void detach_model();
  // методы-утилиты сущности view
  void render();
private:
  custom_model_t* model{nullptr};
private: // отключаем конструктор и оператор копирования (слишком простая реализация)
  console_view_t(const console_view_t&) = delete;
  const console_view_t& operator=(const console_view_t&) = delete;
};

/*! \brief Кастомный файл
*
* Файл реализует интерфейсы \ref import_interface_t и \ref export_interface_t,
* что позволяет ему (файлу) предоставлять методы для чтения и записи данных. Файл
* не простой, а консольный. Это значит что вывод информации о графических
* примитивах попадёт в листинг текстового терминала, а ввод данных будет
* запрашиваться прямо из консоли.
*
* - \see custom_document_t
* - \see primitive_t
*/
class console_file_t : public file_t
{
public:
  console_file_t() = default;
  virtual ~console_file_t() = default;
  void open(const char* disclaimer);
  void close();
protected: // реализация методов интерфейса import_interface_t
  virtual void read(int&) const override;
  virtual void read(unsigned&) const override;
protected: // реализация методов интерфейса export_interface_t
  virtual void write(const int&) override;
  virtual void write(const unsigned&) override;
private: // отключаем конструктор и оператор копирования (слишком простая реализация)
  console_file_t(const console_file_t&) = delete;
  const console_file_t& operator=(const console_file_t&) = delete;
};

/*! \brief Кастомный документ
*
* Является расширением кастомной модели \ref custom_model_t и документа
* \ref document_t. Предоставляет доступ к набору графических примитивов с
* помощью реализации model-сущности, потомком которой является. Ведёт себя
* как документ с графическими примитивами.
*
* - \see custom_model_t
* - \see editor_t
*/
class custom_document_t : public document_t, public custom_model_t
{
public:
  custom_document_t() = default;
  virtual ~custom_document_t() = default;
public: // реализация интерфейса document_t
  virtual bool export_to_file(file_t*) const override;
  virtual void import_from_file(const file_t*) override;
private: // отключаем конструктор и оператор копирования (слишком простая реализация)
  custom_document_t(const custom_document_t&) = delete;
  const custom_document_t& operator=(const custom_document_t&) = delete;
};

/*! \brief Кастомный простейший графический векторный редактор
*
* Является расширением интерфейса \ref editor_t, который в свою очередь
* является контроллером (controller), управляющим моделью \ref custom_model_t и
* предоставляющим доступ к рендерингу с помощью \ref console_view_t.
*
* Представление (view) - часть приложения графического редактора, и когда
* пользовательский код просит выполнить рендеринг, приложение перенаправляет
* вызовы в представление \ref view.
*
* Модель (model) - документ с графическими примитивами, и когда пользовательский
* код просит выполнить какие-то действия с набором графических объектов, приложение
* перенаправляет вызовы в модель, т.е. прямо в документ.
*
* Контроллер (controller) - это и есть графический редактор, который реализует
* требования внешнего API, опубликованных в классе \ref controller_t, и
* предоставляет опосредованную возможность управлять model и view, без
* необходимости осуществления прямого доступа к ним.
*
* Реализует шаблон проектирования "одиночка", из того предположения, что
* являясь реализацией приложения существует "всё время в единственном
* экземпляре".
*
* - \see custom_document_t
* - \see custom_model_t
* - \see console_view_t
* - \see controller_t
*/
class custom_editor_t : public editor_t
{
public:
  static custom_editor_t& instance() { static custom_editor_t self; return self; }

public: // реализация интерфейса controller_t
  virtual void render() override;
  virtual primitive_t* add_circle(int x, int y, unsigned radius) override;
  virtual primitive_t* add_rectangle(int left, int top, unsigned width, unsigned height) override;
  virtual void move(primitive_t* const primitive, int offset_x, int offset_y) override;
  virtual void remove(primitive_t* const primitive) override;

private: // утилиты
  console_view_t* get_view();
  custom_model_t* get_model();

private: // реализация интерфейса editor_t
  custom_editor_t();
  const custom_editor_t& operator=(const custom_editor_t&) = delete;

  virtual document_t* internal_create_document() const override;
  virtual void internal_delete_document(document_t*) const override;

  std::unique_ptr<console_view_t> view;
};

}

/*! @} */

// -*- mode: c++; coding: utf-8 -*-

#include <iostream>

#include "application_specific.h"


using namespace homework13;

int main()
{
  custom_editor_t::instance().create_document();
  custom_editor_t::instance().render();
  // нарисуем снеговика
  custom_editor_t::instance().add_circle(5, 5, 5);
  custom_editor_t::instance().add_circle(5, 14, 4);
  primitive_t * head = 
  custom_editor_t::instance().add_circle(5, 20, 3);
  custom_editor_t::instance().render();
  // исправим голову снеговика
  custom_editor_t::instance().move(head, 0, 1);
  custom_editor_t::instance().render();
  // возъмём снеговика в рамку
  custom_editor_t::instance().add_rectangle(0, 24, 10, 24);
  custom_editor_t::instance().render();
  // убираем у снеговика голову
  custom_editor_t::instance().remove(head);
  custom_editor_t::instance().render();

  // надоел снеговик, загружаем новый документ из файла
  {
    console_file_t file;
    file.open(
      "\n\nThis is document' file emulator. Let's assume you will generate an file stream data.\n"
      "I will ask you and you will answer...\n"
    );
    custom_editor_t::instance().import_from_file(&file);
    file.close();
  }
  // и отображаем загруженный документ с помощью view
  custom_editor_t::instance().render();
  // добавляем ещё один примитив
  custom_editor_t::instance().add_rectangle(-1, -1, 2, 2);
  custom_editor_t::instance().render();

  // сохраняем примитивы документа в файл
  {
    console_file_t file;
    file.open(
      "\n\nCongratulations! We did it ;)\n"
      "Let's see what appears in the file stream?...\n"
    );
    custom_editor_t::instance().export_to_file(&file);
    file.close();
  }

  return EXIT_SUCCESS;
}

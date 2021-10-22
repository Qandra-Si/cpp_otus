set(TARGET "${CMAKE_PROJECT_NAME}_homework01")

# собираем простейшее приложение, которое выводит пару строк в терминал
add_executable(${TARGET} main.cpp)

# добавляем тесты с использованием пакета ctest
add_subdirectory(ctest)

# добавляем тесты на базе библиотеки gtest (если система тестирования не отключена)
if (GTEST_FOUND)
  add_subdirectory(gtest)
endif()

# создаём инсталляционный пакет, в который включаем target-приложение
# переменованное в homework_cli (это название задано в спецификации на выполнение д.з.)
# поскольку нельзя переименовать target (cmake это запрещает), то подключаем к
# инсталляционному пакету собранное приложение по известному пути, и тогда уже переименовываем
if (WIN32)
  install(
    PROGRAMS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/${TARGET}.exe"
    DESTINATION bin CONFIGURATIONS Release RENAME helloworld_cli.exe)
else()
  message(STATUS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}")
  install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
  # в linux такой фокус не пройдёт (почему... не понял), поэтому просто создаю символическую ссылку
  # на несуществующее положение и просто включаю её в дистрибутив
  execute_process(COMMAND rm --force helloworld_cli)
  execute_process(COMMAND ln -s "${CPACK_INSTALL_PREFIX}/${CPP_OTUS_INSTALL_BIN_DIR}/${TARGET}" helloworld_cli)
  install(FILES ${CMAKE_BINARY_DIR}/helloworld_cli DESTINATION bin)
endif()

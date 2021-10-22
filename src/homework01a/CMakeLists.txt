set(TARGET "${CMAKE_PROJECT_NAME}_homework01a")

# собираем программу по заданию второй части домашнего задания
add_executable(${TARGET} ip_filter.cpp)
target_link_libraries(${TARGET} core)

# задаём настройки компилятора
set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON)
# включаем параноидальный уровень предупреждений
if (MSVC)
  target_compile_options(${TARGET} PRIVATE /WX) # warnings -> errors
else()
  target_compile_options(${TARGET} PRIVATE -pipe -Wall -Wextra -Werror -pedantic)
endif()

# создаём инсталляционный пакет, в который включаем target-приложение
# переменованное в ip_filter (это название задано в спецификации на выполнение д.з.)
# поскольку нельзя переименовать target (cmake это запрещает), то подключаем к
# инсталляционному пакету собранное приложение по известному пути, и тогда уже переименовываем
if (WIN32)
  install(
    PROGRAMS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/${TARGET}.exe"
    DESTINATION bin CONFIGURATIONS Release RENAME ip_filter.exe)
else()
  message(STATUS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}")
  install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
  # в linux такой фокус не пройдёт (почему... не понял), поэтому просто создаю символическую ссылку
  # на несуществующее положение и просто включаю её в дистрибутив
  execute_process(COMMAND ln -s "${CPACK_INSTALL_PREFIX}/${CPP_OTUS_INSTALL_BIN_DIR}/${TARGET}" ip_filter)
  install(FILES ${CMAKE_BINARY_DIR}/ip_filter DESTINATION bin)
endif()
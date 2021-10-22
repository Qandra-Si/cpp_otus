set(TARGET "${CMAKE_PROJECT_NAME}_homework01a")

# собираем программу по заданию второй части домашнего задания
add_executable(${TARGET} ip_filter.cpp)
target_link_libraries(${TARGET} core)

# задаём настройки компилятора
if (WIN32)
  # GitHub Actions problem:
  # If you want to have /std:c++latest with CMake 3.21 you need to have set(CMAKE_CXX_STANDARD 23).
  # CMake is setting correctly /std:c++20 for set(CMAKE_CXX_STANDARD 20), but as it turns out some
  # headers (<format>, <ranges) are not included by MSVC with /std:c++20
  # see https://github.com/microsoft/STL/issues/1814#issuecomment-845572895
  set_target_properties(${TARGET} PROPERTIES
    CXX_STANDARD 20  # /std:c++latest
    CXX_STANDARD_REQUIRED ON)
  target_compile_options(${TARGET} PRIVATE /std:c++latest)
else()
  set_target_properties(${TARGET} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)
endif()

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
  execute_process(COMMAND rm --force ${CMAKE_BINARY_DIR}/ip_filter)
  execute_process(COMMAND ln -s "${CPACK_INSTALL_PREFIX}/${CPP_OTUS_INSTALL_BIN_DIR}/${TARGET}" ${CMAKE_BINARY_DIR}/ip_filter)
  install(FILES ${CMAKE_BINARY_DIR}/ip_filter DESTINATION bin)
endif()

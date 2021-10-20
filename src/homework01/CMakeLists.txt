set(TARGET "${CMAKE_PROJECT_NAME}_homework01")

# собираем простейшее приложение, которое выводит пару строк в терминал
add_executable(${TARGET} main.cpp)

# добавляем тест, который будет проверять то, что выводит программа
add_test(NAME test_homework01 COMMAND ${TARGET})
# настраиваем поведение теста
set_tests_properties(
  test_homework01 PROPERTIES
  PASS_REGULAR_EXPRESSION "build 0.10.21\nHello, World!"
)

#TODO: весь следующий код надо перенести в отдельный cmake макрос!
if (GTEST_FOUND)
  # добавляем тест на базе библиотеки gtest
  add_executable(${TARGET}_gtest main_gtest.cpp gtest_homework01.cpp)
  target_include_directories(${TARGET}_gtest PRIVATE ${GTEST_INCLUDE_DIRS})
  gtest_add_tests(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}_gtest "--gtest_color=on" main_gtest.cpp gtest_homework01.cpp)
  target_link_libraries(${TARGET}_gtest ${GTEST_LIBRARIES})
  if (NOT WIN32)
    target_link_libraries(${TARGET}_gtest ${CMAKE_THREAD_LIBS_INIT})
  endif()
  cpp_otus_copy_prerequisites_for_target(${TARGET}_gtest)
endif()

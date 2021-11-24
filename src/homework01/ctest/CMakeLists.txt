# добавляем тест, который будет проверять то, что выводит программа?
add_test(NAME ctest_homework01 COMMAND ${CMAKE_PROJECT_NAME}_homework01)
# настраиваем поведение теста: не будем усложнять, проверим вывод программы прямо в cmake
set_tests_properties(
  ctest_homework01 PROPERTIES
  PASS_REGULAR_EXPRESSION "build 0.11.24\nHello, World!"
)

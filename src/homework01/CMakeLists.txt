set(TARGET "${CMAKE_PROJECT_NAME}_homework01")

# собираем простейшее приложение, которое выводит пару строк в терминал
add_executable(${TARGET} main.cpp)

# добавляем тест, который будет проверять то, что выводит программа
add_test(NAME test_homework01 COMMAND ${TARGET})
# настраиваем поведение теста
set_tests_properties(
  test_homework01 PROPERTIES
  PASS_REGULAR_EXPRESSION "build 0.10.20\nHello, World!"
)

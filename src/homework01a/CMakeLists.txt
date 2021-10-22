set(TARGET "${CMAKE_PROJECT_NAME}_homework01a")

# собираем программу по заданию второй части домашнего задания
add_executable(${TARGET} ip_filter.cpp)

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

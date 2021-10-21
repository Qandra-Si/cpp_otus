include(CppOtusHelpers)

#####################################################################
# cpp_otus_add_test - добавляет gtest-ы и инициализируем их
#####################################################################

macro(cpp_otus_add_test TARGET_NAME)
  include_directories( ${CMAKE_CURRENT_BINARY_DIR}/tests/)
  set(GET_TEST_DIR_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/tests/test_dir_${TARGET_NAME}.h)
  file(WRITE  ${GET_TEST_DIR_SOURCE} "#pragma once\n\n")
  file(APPEND ${GET_TEST_DIR_SOURCE} "#define CPP_OTUS_WRAP_TEST_FILE(fname) \"${CMAKE_CURRENT_BINARY_DIR}/tests/test_dir_${TARGET_NAME}/\" fname\n")
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests/test_dir_${TARGET_NAME})

  set(GET_TARGET_SRCS ${ARGN})
  if (GTEST_FOUND)
    include_directories(${GTEST_INCLUDE_DIRS})
  endif()

  add_executable(${TARGET_NAME} ${GET_TARGET_SRCS})

  if (GTEST_FOUND)
    gtest_add_tests(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME} "--gtest_color=on" ${GET_TARGET_SRCS} )
    target_link_libraries(${TARGET_NAME} ${GTEST_LIBRARIES})
    if( NOT WIN32 )
      target_link_libraries(${TARGET_NAME} ${CMAKE_THREAD_LIBS_INIT})
    endif()
  endif()
endmacro()

#####################################################################
# cpp_otus_test_required_files - копирует в директорию сборки внешние
# файлы, необходимые для проведения тестов
#####################################################################

macro(cpp_otus_test_required_files)
  set(GET_TEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/tests/test_dir_${TARGET_NAME})
  # файл только копируется, и не перезаписывается, если уже есть одноимённый
  foreach(FILENAME ${ARGN})
    get_filename_component(ONLY_FILENAME ${FILENAME} NAME)
    if (NOT EXISTS ${GET_TEST_DIR}/${ONLY_FILENAME})
      file (COPY ${FILENAME} DESTINATION ${GET_TEST_DIR})
    endif()
  endforeach(FILENAME)
endmacro()

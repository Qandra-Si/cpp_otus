get_filename_component(TARGET_SUBNAME ${CMAKE_CURRENT_LIST_DIR} NAME)
set(TARGET "${CMAKE_PROJECT_NAME}_${TARGET_SUBNAME}")

file( GLOB_RECURSE TARGET_HEADERS *.h)
file( GLOB_RECURSE TARGET_SRCS *.cpp )

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(${TARGET} ${TARGET_HEADERS} ${TARGET_SRCS})
target_link_libraries(${TARGET} ${Boost_LIBRARIES}) # используй CPP_OTUS_ENABLE_BOOST=TRUE

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON)

cpp_otus_copy_prerequisites_for_target(${TARGET})

# создаём инсталляционный пакет, в который включаем target-приложение
# переменованное в bayan (это название задано в спецификации на выполнение д.з.)
# поскольку нельзя переименовать target (cmake это запрещает), то подключаем к
# инсталл¤ционному пакету собранное приложение по известному пути, и тогда уже переименовываем
if (WIN32)
  install(
    PROGRAMS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/${TARGET}.exe"
    DESTINATION bin CONFIGURATIONS Release RENAME bayan.exe)
else()
  message(STATUS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}")
  install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
  # в linux такой фокус не пройдёт (почему... не понял), поэтому просто создаю символическую ссылку
  # на несуществующее положение и просто включаю её в дистрибутив
  execute_process(COMMAND rm --force ${CMAKE_BINARY_DIR}/bayan)
  execute_process(COMMAND ln -s "${CPACK_INSTALL_PREFIX}/${CPP_OTUS_INSTALL_BIN_DIR}/${TARGET}" ${CMAKE_BINARY_DIR}/bayan)
  install(FILES ${CMAKE_BINARY_DIR}/bayan DESTINATION bin)
endif()

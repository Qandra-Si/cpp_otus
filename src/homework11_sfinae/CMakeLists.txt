get_filename_component(TARGET_SUBNAME ${CMAKE_CURRENT_LIST_DIR} NAME)
set(TARGET "${CMAKE_PROJECT_NAME}_${TARGET_SUBNAME}")

add_executable(${TARGET} main.cpp)
target_link_libraries(${TARGET})

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON)
# target_compile_options(${TARGET} PRIVATE -Wno-attributes)  # -O3

# создаЄм инсталл€ционный пакет, в который включаем target-приложение
# переменованное в print_ip (это название задано в спецификации на выполнение д.з.)
# поскольку нельз€ переименовать target (cmake это запрещает), то подключаем к
# инсталл€ционному пакету собранное приложение по известному пути, и тогда уже переименовываем
if (WIN32)
  install(
    PROGRAMS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/${TARGET}.exe"
    DESTINATION bin CONFIGURATIONS Release RENAME print_ip.exe)
else()
  message(STATUS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}")
  install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
  # в linux такой фокус не пройдЄт (почему... не пон€л), поэтому просто создаю символическую ссылку
  # на несуществующее положение и просто включаю еЄ в дистрибутив
  execute_process(COMMAND rm --force ${CMAKE_BINARY_DIR}/print_ip)
  execute_process(COMMAND ln -s "${CPACK_INSTALL_PREFIX}/${CPP_OTUS_INSTALL_BIN_DIR}/${TARGET}" ${CMAKE_BINARY_DIR}/print_ip)
  install(FILES ${CMAKE_BINARY_DIR}/print_ip DESTINATION bin)
endif()

get_filename_component(TARGET_SUBNAME ${CMAKE_CURRENT_LIST_DIR} NAME)
set(TARGET "${CMAKE_PROJECT_NAME}_${TARGET_SUBNAME}")

add_executable(${TARGET} main.cpp)
target_link_libraries(${TARGET})

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON)
# target_compile_options(${TARGET} PRIVATE -Wno-attributes)  # -O3

# ������ ��������������� �����, � ������� �������� target-����������
# �������������� � print_ip (��� �������� ������ � ������������ �� ���������� �.�.)
# ��������� ������ ������������� target (cmake ��� ���������), �� ���������� �
# ���������������� ������ ��������� ���������� �� ���������� ����, � ����� ��� ���������������
if (WIN32)
  install(
    PROGRAMS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/${TARGET}.exe"
    DESTINATION bin CONFIGURATIONS Release RENAME print_ip.exe)
else()
  message(STATUS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}")
  install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
  # � linux ����� ����� �� ������ (������... �� �����), ������� ������ ������ ������������� ������
  # �� �������������� ��������� � ������ ������� � � �����������
  execute_process(COMMAND rm --force ${CMAKE_BINARY_DIR}/print_ip)
  execute_process(COMMAND ln -s "${CPACK_INSTALL_PREFIX}/${CPP_OTUS_INSTALL_BIN_DIR}/${TARGET}" ${CMAKE_BINARY_DIR}/print_ip)
  install(FILES ${CMAKE_BINARY_DIR}/print_ip DESTINATION bin)
endif()

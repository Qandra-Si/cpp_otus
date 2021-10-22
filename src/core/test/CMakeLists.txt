set(TARGET_NAME test_core)

file (GLOB_RECURSE TARGET_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hh *.hpp)

cpp_otus_add_test(${TARGET_NAME} ${TARGET_SRCS})
target_link_libraries(${TARGET_NAME} core)
cpp_otus_copy_prerequisites_for_target(${TARGET_NAME})

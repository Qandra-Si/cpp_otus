set(TARGET_NAME ${CMAKE_PROJECT_NAME}_homework01_gtest)

file (GLOB_RECURSE TARGET_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hh *.hpp)

cpp_otus_add_test(${TARGET_NAME} ${TARGET_SRCS})
cpp_otus_copy_prerequisites_for_target(${TARGET_NAME})

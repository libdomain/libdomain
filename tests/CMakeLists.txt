if (NOT ${LIBDOMAIN_BUILD_TESTS})
  return()
endif()

macro(add_libdomain_test test_executable sources)
  set(test_name_local "test.${test_executable}")
  add_executable(${test_executable} ${sources})
  add_test(NAME ${test_name_local} COMMAND ${test_executable})
endmacro(add_libdomain_test)

add_subdirectory(auto)

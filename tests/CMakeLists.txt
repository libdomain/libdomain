if (NOT ${LIBDOMAIN_BUILD_TESTS})
  return()
endif()

macro(add_libdomain_test test_executable sources)
  set(test_name_local "test.${test_executable}")
  add_executable(${test_executable} ${sources})
  add_test(NAME ${test_name_local} COMMAND ${test_executable})

  # set permissions
  install(TARGETS ${test_executable} DESTINATION bin
      PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

  # copy binary
  add_custom_command(TARGET ${test_executable} POST_BUILD COMMAND
      ${CMAKE_COMMAND} ARGS -E copy
        $<TARGET_FILE:${test_executable}>
        ${CMAKE_BINARY_DIR}/bin/$<TARGET_FILE_NAME:${test_executable}>
      COMMENT "Copy ${test_executable} to ${CMAKE_BINARY_DIR}/bin")
endmacro(add_libdomain_test)

add_subdirectory(auto)

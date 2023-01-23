macro(add_libdomain_library name)
  # add library
  add_library(${name} SHARED ${ARGN})

  # add install target
  install(TARGETS ${name} DESTINATION ${CMAKE_INSTALL_LIBDIR}
      PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

  # copy library into build directory
  add_custom_command(TARGET ${name} POST_BUILD COMMAND
      ${CMAKE_COMMAND} ARGS -E copy
        $<TARGET_FILE:${name}>
        ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/$<TARGET_FILE_NAME:${name}>
      COMMENT "Copying library ${name} to output directory")

endmacro(add_libdomain_library)

add_libdomain_library(domain SHARED main.c)

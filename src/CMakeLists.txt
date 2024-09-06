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

find_package(Ldap REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Glib20 REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(Talloc REQUIRED IMPORTED_TARGET talloc)
pkg_check_modules(Libverto REQUIRED IMPORTED_TARGET libverto)
pkg_check_modules(Libconfig REQUIRED IMPORTED_TARGET libconfig)

set(PROJECT_SOURCES
    ad_schema.c
    attribute.c
    attribute.h
    common.c
    common.h
    computer.c
    computer.h
    connection.c
    connection.h
    connection_state_machine.c
    connection_state_machine.h
    directory.c
    directory.h
    domain.h
    domain_p.h
    domain.c
    entry.c
    entry.h
    entry_p.h
    group.c
    group.h
    ldap_parsers.h
    ldap_parsers.c
    ldap_syntaxes.c
    ldap_syntaxes.h
    organizational_unit.c
    organizational_unit.h
    request_queue.h
    request_queue.c
    schema.h
    schema_p.h
    schema.c
    openldap_schema.c
    user.c
    user.h
)

add_subdirectory(syntaxes)
add_subdirectory(parsers)

add_libdomain_library(domain SHARED ${PROJECT_SOURCES})
target_link_libraries(domain PUBLIC PkgConfig::Glib20 PkgConfig::Talloc PkgConfig::Libverto PkgConfig::Libconfig Ldap::Ldap)
target_link_libraries(domain PRIVATE syntax)
target_link_libraries(domain PRIVATE parser)
set_target_properties(domain PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
)

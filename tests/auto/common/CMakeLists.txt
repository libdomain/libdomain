find_package(cgreen REQUIRED)
find_package(Ldap REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Talloc REQUIRED IMPORTED_TARGET talloc)
pkg_check_modules(Libverto REQUIRED IMPORTED_TARGET libverto)

include_directories(${CGREEN_INCLUDE_DIRS})

set(LIBRARY_NAME test-common)

set(SOURCES
    test_common.h
    test_common.c
)

add_library(${LIBRARY_NAME} STATIC ${SOURCES})
target_link_libraries(${LIBRARY_NAME} ${CGREEN_LIBRARIES})
target_link_libraries(${LIBRARY_NAME} domain)
target_link_libraries(${LIBRARY_NAME} Ldap::Ldap)
target_link_libraries(${LIBRARY_NAME} PkgConfig::Libverto)
target_link_libraries(${LIBRARY_NAME} PkgConfig::Talloc)
set_target_properties(${LIBRARY_NAME} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(cgreen REQUIRED)
find_package(Ldap REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Talloc REQUIRED IMPORTED_TARGET talloc)
pkg_check_modules(Libverto REQUIRED IMPORTED_TARGET libverto)

include_directories(${CGREEN_INCLUDE_DIRS})

set(TEST_NAME schema)

set(SOURCES
    schema_tests.h
    schema_new.c
    schema_attributetype.c
    schema_objectclass.c
    schema.c
)

add_libdomain_test(${TEST_NAME} "${SOURCES}")
target_link_libraries(${TEST_NAME} ${CGREEN_LIBRARIES})
target_link_libraries(${TEST_NAME} domain test-common)
target_link_libraries(${TEST_NAME} Ldap::Ldap)
target_link_libraries(${TEST_NAME} PkgConfig::Libverto)
target_link_libraries(${TEST_NAME} PkgConfig::Talloc)


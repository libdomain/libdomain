find_package(cgreen REQUIRED)
find_package(Ldap REQUIRED)
find_package(LibEvent REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Talloc REQUIRED IMPORTED_TARGET talloc)

include_directories(${CGREEN_INCLUDE_DIRS})

set(TEST_NAME configure)

set(SOURCES
    configure.c
)

add_libdomain_test(${TEST_NAME} ${SOURCES})
target_link_libraries(${TEST_NAME} ${CGREEN_LIBRARIES})
target_link_libraries(${TEST_NAME} domain)
target_link_libraries(${TEST_NAME} Ldap::Ldap)
target_link_libraries(${TEST_NAME} LibEvent::LibEvent)
target_link_libraries(${TEST_NAME} PkgConfig::Talloc)

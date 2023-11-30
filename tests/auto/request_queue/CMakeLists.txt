find_package(cgreen REQUIRED)
find_package(Ldap REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Talloc REQUIRED IMPORTED_TARGET talloc)
pkg_check_modules(Libverto REQUIRED IMPORTED_TARGET libverto)
pkg_check_modules(Libconfig REQUIRED IMPORTED_TARGET libconfig)

include_directories(${CGREEN_INCLUDE_DIRS})

set(TEST_NAME request_queue)

set(SOURCES
    request_queue_new.c
    request_queue_push.c
    request_queue_pop.c
    request_queue_empty.c
    request_queue_move.c
    request_queue.c
    request_queue_tests.h
)

add_libdomain_test(${TEST_NAME} "${SOURCES}")
target_link_libraries(${TEST_NAME} ${CGREEN_LIBRARIES})
target_link_libraries(${TEST_NAME} domain test-common)
target_link_libraries(${TEST_NAME} Ldap::Ldap)
target_link_libraries(${TEST_NAME} PkgConfig::Libverto)
target_link_libraries(${TEST_NAME} PkgConfig::Libconfig)
target_link_libraries(${TEST_NAME} PkgConfig::Talloc)

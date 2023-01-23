cmake_minimum_required(VERSION 3.12)

project(libdomain C)

execute_process(
    COMMAND
    bash -c "grep 'Version:' .gear/libdomain.spec | awk -F ':' '{ print $2 }' | tr -d [:space:]"
    OUTPUT_VARIABLE LIB_DOMAIN_GEAR_VERSION
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

string(REPLACE "." ";" GEAR_VERSION_LIST ${LIB_DOMAIN_GEAR_VERSION})
list(GET GEAR_VERSION_LIST 0 LIB_DOMAIN_VERSION_MAJOR)
list(GET GEAR_VERSION_LIST 1 LIB_DOMAIN_VERSION_MINOR)
list(GET GEAR_VERSION_LIST 2 LIB_DOMAIN_VERSION_PATCH)

if(UNIX)
  include(GNUInstallDirs)
else()
    message(FATAL_ERROR "System not UNIX - not implemented yet")
endif()

add_subdirectory(src)

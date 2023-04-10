# Finds LibEvent C Libraries
#
# Defined vars:
#     LibEvent_FOUND
#     LibEvent_INCLUDE_DIRS
# 
# Imported targets:
#     LibEvent::LibEvent

find_path(LIBEVENT_INCLUDE_DIR
  NAMES
       event.h
  PATHS
       /usr/local
       /opt
  PATH_SUFFIXES
       include
)

find_library(LIBEVENT_LIB
  NAMES
      event
  PATHS
      /usr/local
      /opt
  PATH_SUFFIXES
      lib
      lib64
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    FOUND_VAR LibEvent_FOUND
    REQUIRED_VARS
        LIBEVENT_LIB
        LIBEVENT_INCLUDE_DIR
)

if(LibEvent_FOUND)
    set(LibEvent_INCLUDE_DIRS ${LIBEVENT_INCLUDE_DIR})
    list(REMOVE_DUPLICATES LibEvent_INCLUDE_DIRS)
endif()

if(LibEvent_FOUND AND NOT TARGET LibEvent::LibEvent)
    add_library(Ldap::Ldap INTERFACE IMPORTED)
    
    target_link_libraries(LibEvent::LibEvent
        INTERFACE
            event
    )

    target_include_directories(LibEvent::LibEvent
        INTERFACE
            ${LibEvent_INCLUDE_DIRS}
    )
endif()

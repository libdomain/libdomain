set(RL_EXECUTABLE
    ragel
)

file(GLOB RL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.rl")

foreach(FILE ${RL_FILES})
    string(REGEX REPLACE "[.]rl" ".c" SOURCE ${FILE})

    add_custom_command(
        OUTPUT  "${SOURCE}"
        COMMAND ${RL_EXECUTABLE} ${RL_FLAGS} ${FILE}
        DEPENDS ${FILE}
        COMMENT "Running ${RL_EXECUTABLE} ${RL_FLAGS} ${FILE}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND_EXPAND_LISTS
        VERBATIM
    )

    list(APPEND OUT_FILES "${SOURCE}")
endforeach()

set(RL_FLAGS
    -C
    -T0
)

set(SYNTAX_LIBRARY_SOURCES
    syntaxes.h
)

add_custom_target(rl_files ALL SOURCES ${RL_FILES})
add_custom_target(generate_all_files ALL DEPENDS ${OUT_FILES})

add_library(syntax STATIC ${SYNTAX_LIBRARY_SOURCES} ${OUT_FILES})
set_property(TARGET syntax PROPERTY POSITION_INDEPENDENT_CODE ON)
add_dependencies(syntax generate_all_files)

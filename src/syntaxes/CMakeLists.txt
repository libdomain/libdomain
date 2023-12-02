set(RL_EXECUTABLE
    ragel
)

set(RL_FILES
    ia5string.rl
    integer.rl
)

foreach(FILE ${RL_FILES})
    string(REGEX REPLACE "[.]rl" ".c" SOURCE ${FILE})

    add_custom_command(
        OUTPUT  "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}"
        COMMAND ${RL_EXECUTABLE} ${RL_FLAGS} ${FILE}
        DEPENDS ${FILE}
        COMMENT "Running ${RL_EXECUTABLE} ${RL_FLAGS} ${FILE}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND_EXPAND_LISTS
        VERBATIM
    )

    list(APPEND OUT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}")
endforeach()

set(RL_FLAGS
    -C
    -G2
)

set(SYNTAX_LIBRARY_SOURCES
    syntaxes.h
)

add_custom_target(generate_all_files ALL DEPENDS ${OUT_FILES})

add_library(syntax SHARED ${SYNTAX_LIBRARY_SOURCES} ${OUT_FILES})
add_dependencies(syntax generate_all_files)

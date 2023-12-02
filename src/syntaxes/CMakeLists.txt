set(RL_EXECUTABLE
    ragel
)

set(RL_FILES
    ia5string.rl
)

foreach(FILE ${RL_FILES})
    string(REGEX REPLACE "[.]rl" ".c" SOURCE ${FILE})
    set(C_SOURCES ${C_SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}")
endforeach()

set(OUT_FILES
    ${C_SOURCES}
)

set(RL_FLAGS
    -C
    -G2
)

add_custom_command(
    OUTPUT ${OUT_FILES}
    COMMAND ${RL_EXECUTABLE} ${RL_FLAGS} ${RL_FILES}
    DEPENDS ${RL_FILES}
    COMMENT "Running ${RL_EXECUTABLE} ${RL_FLAGS} ${RL_FILES}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND_EXPAND_LISTS
)

set(SYNTAX_LIBRARY_SOURCES
    syntaxes.h
)

add_library(syntax STATIC ${OUT_FILES} ${SYNTAX_LIBRARY_SOURCES})

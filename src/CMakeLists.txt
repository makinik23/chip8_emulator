add_subdirectory(display)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Chip8.cpp
    ${DISPLAY_SOURCES}
    PARENT_SCOPE
)

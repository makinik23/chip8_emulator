cmake_minimum_required(VERSION 3.10)
project(Chip8Emulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(include)
add_subdirectory(src)

link_directories(${CMAKE_SOURCE_DIR}/libs/SDL2/lib)

add_executable(chip8_emulator ${SOURCES})

target_include_directories(chip8_emulator
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/libs/SDL2/include/SDL2
)

target_link_libraries(chip8_emulator
    mingw32
    SDL2main
    SDL2
    setupapi
    imm32
    version
    winmm
)

add_custom_command(TARGET chip8_emulator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/roms $<TARGET_FILE_DIR:chip8_emulator>/roms
)

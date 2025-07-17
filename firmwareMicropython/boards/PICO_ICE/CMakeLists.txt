cmake_minimum_required(VERSION 3.12)

get_filename_component(PROJECT_DIR "../.." ABSOLUTE)
set(MICROPY_PORT_DIR ${PROJECT_DIR}/lib/micropython/ports/rp2)
get_filename_component(MICROPY_BOARD_DIR "." ABSOLUTE)

if (NOT DEFINED PICO_BOARD OR PICO_BOARD STREQUAL pico_ice)
    set(PICO_BOARD pico_ice)
    set(MICROPY_BOARD pico_ice)
    add_compile_definitions(PICO_ICE)
endif()
if (PICO_BOARD STREQUAL pico2_ice)
    if (NOT DEFINED PICO_PLATFORM)
        set(PICO_PLATFORM rp2350)
    endif()
    set(MICROPY_BOARD pico_ice)
    set(PICO_NUM_GPIOS 48)
    add_compile_definitions(PICO2_ICE)
endif()

include(${MICROPY_BOARD_DIR}/mpconfigboard.cmake)
add_subdirectory(${MICROPY_PORT_DIR} micropython)
add_dependencies(BUILD_VERSION_HEADER pioasm)

project(micropython)

add_custom_target(copy_uf2 ALL)
add_dependencies(copy_uf2 firmware)

add_custom_command(TARGET copy_uf2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_BINARY_DIR}/micropython/firmware.uf2
	${CMAKE_BINARY_DIR}/firmware.uf2)

add_custom_command(TARGET copy_uf2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --green "${CMAKE_BINARY_DIR}/firmware.uf2 is ready!")

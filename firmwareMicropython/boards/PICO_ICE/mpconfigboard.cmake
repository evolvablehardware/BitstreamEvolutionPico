# CMake file for tinyVision.ai boards

# The tinyVision.ai boards don't have official pico-sdk support so we define it
# See also: https://github.com/raspberrypi/pico-sdk/tree/master/src/boards/include/boards
list(APPEND PICO_BOARD_HEADER_DIRS ${MICROPY_DIR}/../pico-ice-mpy-module/pico-ice-sdk/include/boards)

# Freeze board.py
set(MICROPY_FROZEN_MANIFEST ${MICROPY_BOARD_DIR}/manifest.py)

set(USER_C_MODULES
    ${PROJECT_DIR}/lib/pico-ice-mpy-module/micropython.cmake
)

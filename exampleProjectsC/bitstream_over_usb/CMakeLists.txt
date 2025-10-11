cmake_minimum_required(VERSION 3.13)

include(${CMAKE_CURRENT_SOURCE_DIR}/pico-ice-sdk/cmake/preinit_pico_ice_sdk.cmake)

# import the pico-sdk
set(PICO_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk/)
include(pico_sdk_import.cmake)

# configure the pico-sdk project
project(bitstream_over_usb C CXX ASM)
pico_sdk_init()

# add the pico-ice-sdk
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/pico-ice-sdk)

# add the local files
add_executable(${CMAKE_PROJECT_NAME}
    main.cpp
    usb_descriptors.c
    )
target_link_libraries(${CMAKE_PROJECT_NAME}
    pico_ice_sdk
    pico_ice_usb
    )
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})
pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 0)  # or 1 if you want UART enabled too


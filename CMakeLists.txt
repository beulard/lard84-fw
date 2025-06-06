# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD solderparty_rp2350_stamp CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(lard84-fw C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(lard84-fw src/lard84_main.c src/lard84_usb.c src/lard84_keymatrix.c src/lard84_keycodes.c)

target_compile_definitions(lard84-fw PRIVATE
    PICO_DEFAULT_UART_TX_PIN=12
    PICO_DEFAULT_UART_RX_PIN=13
)

pico_set_program_name(lard84-fw "lard84-fw")
pico_set_program_version(lard84-fw "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(lard84-fw 1)
pico_enable_stdio_usb(lard84-fw 0)

# Add the standard library to the build
target_link_libraries(lard84-fw
        pico_stdlib
        pico_multicore
        hardware_pwm
        tinyusb_device
)

# Add the standard include files to the build
target_include_directories(lard84-fw PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(lard84-fw)


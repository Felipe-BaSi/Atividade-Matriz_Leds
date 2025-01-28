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
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(matriz_led C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(matriz_led matriz_led.c )

pico_set_program_name(matriz_led "matriz_led")
pico_set_program_version(matriz_led "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(matriz_led 0)
pico_enable_stdio_usb(matriz_led 0)

pico_generate_pio_header(matriz_led ${CMAKE_CURRENT_LIST_DIR}/pio_matrix.pio)

# Add the standard library to the build
target_link_libraries(matriz_led
        hardware_pwm
        pico_stdlib
        hardware_pio
        hardware_clocks
        hardware_adc
        pico_bootrom)

# Add the standard include files to the build
target_include_directories(matriz_led PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${PICO_SDK_PATH}/src/common/pico_stdlib/include
        ${PICO_SDK_PATH}/src/rp2_common/hardware_gpio/include
        ${PICO_SDK_PATH}/src/rp2_common/hardware_pio/include
        ${PICO_SDK_PATH}/src/rp2_common/hardware_clocks/include
        ${PICO_SDK_PATH}/src/rp2_common/hardware_adc/include
        ${PICO_SDK_PATH}/src/rp2_common/pico_bootrom/include
)

# Add any user requested libraries
target_link_libraries(matriz_led 
        
        )

pico_add_extra_outputs(matriz_led)

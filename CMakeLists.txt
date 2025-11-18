cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(my_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# Create ST7789 library
add_library(pico_st7789 
    ${CMAKE_CURRENT_LIST_DIR}/lib/st7789/st7789.c
)

target_include_directories(pico_st7789 PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/lib/st7789/include
)

target_link_libraries(pico_st7789 PUBLIC
    pico_stdlib 
    hardware_spi
    hardware_dma
)

# Add LVGL library - gather all source files
file(GLOB_RECURSE LVGL_SOURCES 
    ${CMAKE_CURRENT_LIST_DIR}/lib/lvgl/src/*.c
)

add_library(lvgl ${LVGL_SOURCES})

target_include_directories(lvgl PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/lib/lvgl
    ${CMAKE_CURRENT_LIST_DIR}/lib/lvgl/src
)

target_compile_definitions(lvgl PUBLIC LV_CONF_INCLUDE_SIMPLE)

target_link_libraries(lvgl PUBLIC
    pico_stdlib
)

# Main project executable
# Choose which main file to use:
# - main.cpp: Original simple demo
# - main_simple_test.cpp: Simple display test with debug output
# - main_lvgl.cpp: Full LVGL demo (requires more memory)

add_executable(my_project
    src/main.cpp
    src/st7789_lvgl_driver.cpp
    src/debounced_button.cpp
)

# Important: Removed pico_cyw43_arch_none to reduce dependencies and RAM usage
target_link_libraries(my_project 
    pico_stdlib
    pico_st7789           # ST7789 display library
    lvgl                  # LVGL graphics library
)

pico_enable_stdio_usb(my_project 1)
pico_enable_stdio_uart(my_project 0)

pico_add_extra_outputs(my_project)
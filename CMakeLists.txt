cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME oled)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    Core/Src/dma.c
    Core/Src/kk_app.c
    Core/Src/kk_app_port.c
    Kk/kk_oled/driver/kk_oled_driver.c
    Kk/kk_oled/graphics/kk_oled.c
    Kk/kk_oled/graphics/kk_oled_bitmap.c
    Kk/kk_oled/graphics/kk_oled_draw.c
    Kk/kk_oled/graphics/kk_oled_font.c
    Kk/kk_ui/src/kk_ui.c
    Kk/kk_ui/src/kk_ui_anim.c
    Kk/kk_ui/src/kk_ui_dialog.c
    Kk/kk_ui/src/kk_ui_draw.c
    Kk/kk_ui/src/kk_ui_home.c
    Kk/kk_ui/src/kk_ui_menu.c
    Kk/kk_ui/src/kk_ui_nav.c
    Kk/kk_ui/src/kk_ui_page.c
    Kk/kk_demo/src/kk_ui_app.c
    Kk/kk_demo/src/kk_ui_fonts.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    Core/Inc
    Kk/kk_oled/include
    Kk/kk_oled/graphics
    Kk/kk_oled/driver
    Kk/kk_ui/include
    Kk/kk_ui/src
    Kk/kk_demo/include
    Kk/kk_demo/src
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
    KK_UI_REFRESH_MODE=KK_UI_REFRESH_DMA
    KK_UI_ENABLE_CUSTOM=1
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

cmake_minimum_required(VERSION 3.21)
project(DINO_GBA LANGUAGES ASM C)

set(GAME_SOURCES
        source/dino.c
        source/graphics/dinoSheetHelper.c
        source/save.c
        source/hitbox.c
        source/obstacle.c
        source/game.c
        source/horizon.c
        source/meter.c
        source/graphics/graphics.c
        source/input.c
)
add_executable(dino-gba ${GAME_SOURCES})

set(GAME_INCLUDES
        include
        include/graphics
)
target_include_directories(dino-gba PRIVATE ${GAME_INCLUDES})

# gba-toolchain sets `CMAKE_SYSTEM_NAME` to `AdvancedGameBoy`
if(CMAKE_SYSTEM_NAME STREQUAL AdvancedGameBoy)
    find_package(librom REQUIRED) # GBA ROM runtime library
    find_package(tonclib REQUIRED) # Tonc C development library
    find_package(maxmod REQUIRED) # GBA music and sound solution
    find_package(grit REQUIRED) # GBA bitmap converter

    add_maxmod_library(dino_soundbank
            audio/button-pressed.wav
            audio/hit.wav
            audio/score-reached.wav
    )

    add_grit_library(palette_bank PALETTE
            NO_GRAPHICS NO_MAP PALETTE_COUNT 32
            graphics/default.bmp
            graphics/gbc/GBC-ADOWN.bmp
            graphics/gbc/GBC-ALEFT.bmp
            graphics/gbc/GBC-ARIGHT.bmp
            graphics/gbc/GBC-AUP.bmp
            graphics/gbc/GBC-BDOWN.bmp
            graphics/gbc/GBC-BLEFT.bmp
            graphics/gbc/GBC-BRIGHT.bmp
            graphics/gbc/GBC-BUP.bmp
            graphics/gbc/GBC-DOWN.bmp
            graphics/gbc/GBC-LEFT.bmp
            graphics/gbc/GBC-RIGHT.bmp
            graphics/gbc/GBC-UP.bmp
    )

    add_grit_library(dino_sheet NO_PALETTE NO_MAP GRAPHICS_BIT_DEPTH 4
            graphics/dinoSheet.bmp
    )

    target_compile_options(dino-gba PRIVATE
            -mthumb
            -mthumb-interwork
            -g
            -Wall
            -Wno-unknown-pragmas
            -mcpu=arm7tdmi
            -mtune=arm7tdmi
            -gdwarf-4
    )

    if(CMAKE_BUILD_TYPE STREQUAL "debug")
        message("Configuring for Debug build")
        target_compile_options(dino-gba PRIVATE -Og)
    elseif(CMAKE_BUILD_TYPE STREQUAL "release")
        message("Configuring for Release build")
        target_compile_options(dino-gba PRIVATE -O3)
    endif()

    if (MGBA_LOGS STREQUAL "true")
        message("Enabling mGBA logging")
        target_compile_definitions(dino-gba PRIVATE VERBOSE=1)
    endif()

    target_link_libraries(dino-gba PRIVATE librom tonclib maxmod
            dino_soundbank dino_sheet palette_bank)

    # ROM header info
    set_target_properties(dino-gba PROPERTIES
            ROM_TITLE "Dino Advance"
            ROM_MAKER EF
            ROM_VERSION 2
            C_STANDARD 17
    )

    # install to .gba
    install_rom(dino-gba)

endif()

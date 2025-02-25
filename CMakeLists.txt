cmake_minimum_required(VERSION 3.21)
project(DINO_GBA LANGUAGES ASM C)

set(GAME_SOURCES
        source/dino.c
        source/dinoSheet.c
        source/dinoSheetHelper.c
        source/save.c
        source/hitbox.c
        source/obstacle.c
        source/game.c
        source/horizon.c
        source/meter.c
)
add_executable(dino-gba ${GAME_SOURCES})

set(GAME_INCLUDES
        include
)
target_include_directories(dino-gba PRIVATE ${GAME_INCLUDES})

# gba-toolchain sets `CMAKE_SYSTEM_NAME` to `AdvancedGameBoy`
if(CMAKE_SYSTEM_NAME STREQUAL AdvancedGameBoy)
    find_package(librom REQUIRED) # GBA ROM runtime library
    find_package(tonclib REQUIRED) # Tonc C development library
    find_package(agbabi REQUIRED) # GBA optimized functions library
    find_package(maxmod REQUIRED) # GBA music and sound solution

    add_maxmod_library(dino_soundbank
            audio/button-pressed.wav
            audio/hit.wav
            audio/score-reached.wav
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

    target_link_libraries(dino-gba PRIVATE librom tonclib agbabi maxmod dino_soundbank)

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

cmake_minimum_required(VERSION 3.16)

project(PuzzledChess
    VERSION 1.0.0
    DESCRIPTION "Chess puzzle trainer built with raylib and the Lichess puzzle database"
    LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Build configuration
# ---------------------------------------------------------------------------

# Release by default: the CI builds and the published downloads want an
# optimised binary, but Debug stays available via -DCMAKE_BUILD_TYPE=Debug.
get_property(_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT _is_multi_config AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        Debug Release RelWithDebInfo MinSizeRel)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)   # the sources rely on GNU extensions, e.g. bits/stdc++.h

option(PUZZLEDCHESS_WERROR "Treat compiler warnings as errors" OFF)

# The game loads "./assets/..." relative to the working directory, so the
# binary is placed in <build>/bin and the assets are staged next to it.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# ---------------------------------------------------------------------------
# raylib discovery
#
# Tried in order:
#   1. find_package(raylib CONFIG)  - raylib >= 5 installed via CMake
#   2. pkg-config                   - distro packages / Homebrew
#   3. manual header + library lookup
#   4. -DPUZZLEDCHESS_USE_BUNDLED_RAYLIB=ON to compile a pinned copy
# ---------------------------------------------------------------------------

set(PUZZLEDCHESS_USE_BUNDLED_RAYLIB OFF CACHE BOOL
    "Build the bundled raylib sources instead of using an installed raylib")
set(RAYLIB_GIT_TAG "5.5" CACHE STRING "raylib tag used when building from source")

if(PUZZLEDCHESS_USE_BUNDLED_RAYLIB)
    include(FetchContent)
    FetchContent_Declare(raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        ${RAYLIB_GIT_TAG}
        GIT_SHALLOW    TRUE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(raylib)
    add_library(puzzledchess_raylib INTERFACE)
    target_link_libraries(puzzledchess_raylib INTERFACE raylib)
    set(PUZZLEDCHESS_RAYLIB_TARGET puzzledchess_raylib)
else()
    find_package(raylib 5.0 QUIET CONFIG)
    if(raylib_FOUND OR TARGET raylib)
        message(STATUS "raylib: using CMake package config")
        set(PUZZLEDCHESS_RAYLIB_TARGET raylib)
    else()
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(RAYLIB_PC QUIET IMPORTED_TARGET raylib)
        endif()
        if(TARGET PkgConfig::RAYLIB_PC)
            message(STATUS "raylib: using pkg-config (${RAYLIB_PC_VERSION})")
            set(PUZZLEDCHESS_RAYLIB_TARGET PkgConfig::RAYLIB_PC)
        else()
            find_path(RAYLIB_INCLUDE_DIR
                NAMES raylib.h
                HINTS ENV RAYLIB_INSTALL_PATH
                PATH_SUFFIXES include)
            find_library(RAYLIB_LIBRARY
                NAMES raylib libraylib
                HINTS ENV RAYLIB_INSTALL_PATH
                PATH_SUFFIXES lib lib64)
            if(NOT RAYLIB_INCLUDE_DIR OR NOT RAYLIB_LIBRARY)
                message(FATAL_ERROR
                    "raylib was not found.\n"
                    "Install it (Arch: 'pacman -S raylib', Debian/Ubuntu: 'apt install libraylib-dev',\n"
                    "macOS: 'brew install raylib', Windows: 'vcpkg install raylib'), or configure with\n"
                    "  -DPUZZLEDCHESS_USE_BUNDLED_RAYLIB=ON\n"
                    "to compile raylib ${RAYLIB_GIT_TAG} from source automatically.")
            endif()
            message(STATUS "raylib: ${RAYLIB_LIBRARY}")
            add_library(puzzledchess_raylib INTERFACE)
            target_include_directories(puzzledchess_raylib INTERFACE ${RAYLIB_INCLUDE_DIR})
            target_link_libraries(puzzledchess_raylib INTERFACE ${RAYLIB_LIBRARY})
            set(PUZZLEDCHESS_RAYLIB_TARGET puzzledchess_raylib)
        endif()
    endif()
endif()

# ---------------------------------------------------------------------------
# Game
# ---------------------------------------------------------------------------

add_executable(PuzzledChess WIN32 main.cpp)
target_include_directories(PuzzledChess PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(PuzzledChess PRIVATE ${PUZZLEDCHESS_RAYLIB_TARGET})

if(MSVC)
    target_compile_options(PuzzledChess PRIVATE /W3 /utf-8)
    if(PUZZLEDCHESS_WERROR)
        target_compile_options(PuzzledChess PRIVATE /WX)
    endif()
else()
    target_compile_options(PuzzledChess PRIVATE -Wall)
    if(PUZZLEDCHESS_WERROR)
        target_compile_options(PuzzledChess PRIVATE -Werror)
    endif()
endif()

option(PUZZLEDCHESS_STATIC_RUNTIME
    "Produce a fully static, self-contained executable" OFF)
if(MINGW)
    if(PUZZLEDCHESS_STATIC_RUNTIME)
        target_link_options(PuzzledChess PRIVATE -static)
    else()
        target_link_options(PuzzledChess PRIVATE -static-libgcc -static-libstdc++)
    endif()
endif()

# ---------------------------------------------------------------------------
# Assets
# ---------------------------------------------------------------------------

set(PUZZLEDCHESS_ASSET_STAGE "${CMAKE_BINARY_DIR}/bin/assets")

add_custom_command(TARGET PuzzledChess POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${PUZZLEDCHESS_ASSET_STAGE}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets" "${PUZZLEDCHESS_ASSET_STAGE}"
    COMMENT "Staging PuzzledChess assets into ${PUZZLEDCHESS_ASSET_STAGE}"
    VERBATIM)

# ---------------------------------------------------------------------------
# Install / portable bundle
#
#   cmake --install build --prefix dist
#
# produces a self contained folder: dist/bin/PuzzledChess + dist/assets/...
# ---------------------------------------------------------------------------

install(TARGETS PuzzledChess
    RUNTIME DESTINATION bin
    BUNDLE  DESTINATION .)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets/"
    DESTINATION assets
    USE_SOURCE_PERMISSIONS)

# ---------------------------------------------------------------------------
# Summary
# ---------------------------------------------------------------------------

message(STATUS "")
message(STATUS "  ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "  build type : ${CMAKE_BUILD_TYPE}")
message(STATUS "  output     : ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "")
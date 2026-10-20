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

# raylib discovery
#
# Order of preference:
#   1. -DRAYLIB_INCLUDE_DIR=... -DRAYLIB_LIBRARY=...   explicit and most reliable
#   2. -DPUZZLEDCHESS_USE_BUNDLED_RAYLIB=ON                   compile a pinned copy
#   3. find_package(raylib CONFIG)                      raylib 5+ CMake install
#   4. pkg-config                                       distro packages, Homebrew
#   5. find_path + find_library                         manual fallback
#
# pkg-config is deliberately skipped while cross-compiling: it resolves .pc
# files against the build host and happily hands the linker a bare "-lraylib"
# with no matching -L, which fails at link time with a confusing error.
# ---------------------------------------------------------------------------

set(RAYLIB_INCLUDE_DIR "" CACHE PATH "Explicit path to raylib.h")
set(RAYLIB_LIBRARY    "" CACHE FILEPATH "Explicit path to the raylib library")

if(RAYLIB_INCLUDE_DIR AND RAYLIB_LIBRARY AND EXISTS "${RAYLIB_LIBRARY}")
    message(STATUS "raylib: using explicit paths")
    message(STATUS "  include: ${RAYLIB_INCLUDE_DIR}")
    message(STATUS "  library: ${RAYLIB_LIBRARY}")
    add_library(pacman_raylib INTERFACE)
    target_include_directories(pacman_raylib SYSTEM INTERFACE "${RAYLIB_INCLUDE_DIR}")
    target_link_libraries(pacman_raylib INTERFACE "${RAYLIB_LIBRARY}")
    set(PUZZLEDCHESS_RAYLIB_TARGET pacman_raylib)
elseif(PUZZLEDCHESS_USE_BUNDLED_RAYLIB)
    include(FetchContent)
    FetchContent_Declare(raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        ${RAYLIB_GIT_TAG}
        GIT_SHALLOW    TRUE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(raylib)
    add_library(pacman_raylib INTERFACE)
    target_link_libraries(pacman_raylib INTERFACE raylib)
    set(PUZZLEDCHESS_RAYLIB_TARGET pacman_raylib)
else()
    find_package(raylib 5.0 QUIET CONFIG)
    if(TARGET raylib)
        message(STATUS "raylib: using CMake package config")
        set(PUZZLEDCHESS_RAYLIB_TARGET raylib)
    else()
        set(_raylib_found FALSE)
        if(NOT CMAKE_CROSSCOMPILING)
            find_package(PkgConfig QUIET)
            if(PkgConfig_FOUND)
                pkg_check_modules(RAYLIB_PC QUIET IMPORTED_TARGET raylib)
            endif()
            if(TARGET PkgConfig::RAYLIB_PC)
                message(STATUS "raylib: using pkg-config (${RAYLIB_PC_VERSION})")
                set(PUZZLEDCHESS_RAYLIB_TARGET PkgConfig::RAYLIB_PC)
                set(_raylib_found TRUE)
            endif()
        endif()
        if(NOT _raylib_found)
            find_path(RAYLIB_INCLUDE_DIR_FOUND
                NAMES raylib.h
                HINTS ENV RAYLIB_INSTALL_PATH
                PATH_SUFFIXES include)
            find_library(RAYLIB_LIBRARY_FOUND
                NAMES raylib libraylib
                HINTS ENV RAYLIB_INSTALL_PATH
                PATH_SUFFIXES lib lib64)
            if(RAYLIB_INCLUDE_DIR_FOUND AND RAYLIB_LIBRARY_FOUND)
                message(STATUS "raylib: ${RAYLIB_LIBRARY_FOUND}")
                add_library(pacman_raylib INTERFACE)
                target_include_directories(pacman_raylib SYSTEM INTERFACE "${RAYLIB_INCLUDE_DIR_FOUND}")
                target_link_libraries(pacman_raylib INTERFACE "${RAYLIB_LIBRARY_FOUND}")
                set(PUZZLEDCHESS_RAYLIB_TARGET pacman_raylib)
            else()
                message(FATAL_ERROR
                    "raylib was not found.\n\n"
                    "Install it:\n"
                    "  Arch / CachyOS : sudo pacman -S raylib\n"
                    "  Debian / Ubuntu : sudo apt install libraylib-dev\n"
                    "  macOS          : brew install raylib\n\n"
                    "Or point CMake straight at an existing install:\n"
                    "  -DRAYLIB_INCLUDE_DIR=/path/to/include -DRAYLIB_LIBRARY=/path/to/libraylib.a\n\n"
                    "Or let CMake build a pinned copy for you:\n"
                    "  -DPUZZLEDCHESS_USE_BUNDLED_RAYLIB=ON")
            endif()
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
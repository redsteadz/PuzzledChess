# ---------------------------------------------------------------------------
# Cross-compilation toolchain: Windows x86_64 via MinGW-w64
#
# Used by the release workflow to produce native Windows .exe files from a
# Linux runner, which keeps a single CI matrix for both platforms:
#
#   cmake -S . -B build \
#         -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw-x86_64.cmake
#
# The *-posix threading flavour of the compiler is required: raylib's audio and
# GLFW backends expect POSIX threads rather than the win32 model.
# ---------------------------------------------------------------------------

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(MINGW_TRIPLET "x86_64-w64-mingw32" CACHE STRING "MinGW-w64 target triplet")

find_program(CMAKE_C_COMPILER   NAMES ${MINGW_TRIPLET}-gcc-posix   ${MINGW_TRIPLET}-gcc REQUIRED)
find_program(CMAKE_CXX_COMPILER NAMES ${MINGW_TRIPLET}-g++-posix   ${MINGW_TRIPLET}-g++ REQUIRED)
find_program(CMAKE_RC_COMPILER  NAMES ${MINGW_TRIPLET}-windres     ${MINGW_TRIPLET}-windres)

set(CMAKE_FIND_ROOT_PATH /usr/${MINGW_TRIPLET})

# Look for programs on the host, but only ever find libraries/headers/packages
# inside the cross sysroot, otherwise CMake would pick up host .so files.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
#pragma once

// ---------------------------------------------------------------------------
// Windows console bridging
//
// The games are linked as GUI subsystem executables (WIN32 / -mwindows), so a
// double-clicked build shows no console window. That also means anything
// written to stdout/stderr is discarded on Windows, which makes failures hard
// to diagnose.
//
// AttachParentConsole() re-attaches stdout/stderr/stdin to the parent console
// when the game was launched *from* one (cmd.exe, PowerShell, Git Bash, CI
// logs) and does nothing at all when it was double-clicked from Explorer, so
// no extra window ever appears.
// ---------------------------------------------------------------------------

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <cstdio>
#include <windows.h>

inline void AttachParentConsole() {
  // ERROR_ACCESS_DENIED means a console is already attached.
  if (AttachConsole(ATTACH_PARENT_PROCESS) || GetLastError() == ERROR_ACCESS_DENIED) {
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
  }
}

#else

inline void AttachParentConsole() {}

#endif
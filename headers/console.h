#pragma once

// ---------------------------------------------------------------------------
// Windows console bridging
//
// The game is linked as a GUI subsystem executable, so a double-clicked build
// shows no console window. That also means anything written to stdout/stderr is
// discarded on Windows, which makes failures such as a missing puzzle database
// impossible to diagnose.
//
// AttachParentConsole() re-attaches stdout/stderr/stdin to the parent console
// when the game was launched *from* one (cmd.exe, PowerShell, Git Bash, CI
// logs) and does nothing at all when it was double-clicked from Explorer, so no
// extra window ever appears.
//
// Note: <windows.h> is deliberately NOT included here. It declares GDI and USER
// entry points such as Rectangle(), CloseWindow() and DrawText(), which
// collide head-on with raylib's Rectangle typedef and raylib's own functions.
// The two functions below are declared by hand with primitive types so that no
// Windows header is needed.
// ---------------------------------------------------------------------------

#ifdef _WIN32

extern "C" {

// kernel32.dll: BOOL AttachConsole(DWORD dwProcessId), with
// ATTACH_PARENT_PROCESS == 0xFFFFFFFF.
__declspec(dllimport) int __stdcall AttachConsole(unsigned long dwProcessId);

// kernel32.dll: DWORD GetLastError(). ERROR_ACCESS_DENIED == 5.
__declspec(dllimport) unsigned long __stdcall GetLastError();

}  // extern "C"

#include <cstdio>

inline void AttachParentConsole() {
  // ERROR_ACCESS_DENIED means a console is already attached to this process.
  if (AttachConsole(0xFFFFFFFFul) || GetLastError() == 5ul) {
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
// Generated C++ file by Il2CppInspector - http://www.djkaty.com - https://github.com/djkaty
// Custom injected code entry point

#include "pch-il2cpp.h"

// IL2CPP Stuff
#include "il2cpp-appdata.h"
#include "helpers.h"

// Helpers
#include <Utils/gameutility.h>
#include <Log/cheatlog.h>
#include <Hooks/hooks.h>
#include <thread>
#include <chrono>

using namespace app;

// LogFile Name
extern const LPCWSTR LOG_FILE = L"BroCheat.log";

// Custom injected code entry point
DWORD WINAPI MainThread()
{
    // Initialize thread data - DO NOT REMOVE
    il2cpp_thread_attach(il2cpp_domain_get());

    // Open Console and redirect stdout
	il2cppi_new_console();

	DisableAntiCheat();

	// Print Startup Message
	CheatLog::StartupMessage();

	CheatLog::LogMessage("Starting to hook graphics...");
	HookGraphics();
	CheatLog::LogMessage("Hooked graphics!");

	CheatLog::LogMessage("Starting to hook Game Functions...");
	HookGameFunctions();
	CheatLog::LogMessage("Hooked Game Functions!");

	CheatLog::LogMessage("Hooking done!");

	return TRUE;
}
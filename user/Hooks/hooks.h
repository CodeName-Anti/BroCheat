#pragma once
#include <Windows.h>

extern HMODULE baseModule;

HWND GetWindow();

void HookGraphics();

void HookGameFunctions();
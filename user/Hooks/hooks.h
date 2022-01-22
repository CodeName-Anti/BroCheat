#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

HWND GetWindow();

void HookGraphics();

void HookGameFunctions();
#pragma once
#include <windows.h>

bool IsXiaomiHardware();
bool RunCommandHidden(const wchar_t* cmdLine, DWORD& exitCode);

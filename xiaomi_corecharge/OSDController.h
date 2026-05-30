#pragma once
#include <windows.h>

void ShowOSD(int mode);
void RenderOSD(int mode, int alpha);
LRESULT CALLBACK OSDWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

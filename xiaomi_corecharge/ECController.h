#pragma once
#include <windows.h>

// WinRing0 function pointer type definitions
typedef BOOL(WINAPI *InitializeOls_t)();
typedef VOID(WINAPI *DeinitializeOls_t)();
typedef BYTE(WINAPI *ReadIoPortByte_t)(WORD port);
typedef VOID(WINAPI *WriteIoPortByte_t)(WORD port, BYTE value);

extern HMODULE g_hWinRing0;
extern bool g_winRing0Initialized;
extern InitializeOls_t InitializeOls;
extern DeinitializeOls_t DeinitializeOls;
extern ReadIoPortByte_t ReadIoPortByte;
extern WriteIoPortByte_t WriteIoPortByte;

bool LoadWinRing0();
void UnloadWinRing0();
bool InitializeWinRing0();
void DeinitializeWinRing0();
BYTE ReadEC(WORD address);
void WriteEC(WORD address, BYTE value);
void UpdateBatteryCareStatus();
void UpdatePerformanceMode();
void UpdatePerfControls();
void SetBatteryCare(bool enable, int level = 80);
void SetPerformanceMode(int mode);

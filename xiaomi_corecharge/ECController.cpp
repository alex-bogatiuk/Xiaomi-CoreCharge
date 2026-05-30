#include "ECController.h"
#include "Constants.h"
#include "Globals.h"

HMODULE g_hWinRing0 = nullptr;
bool g_winRing0Initialized = false;
InitializeOls_t InitializeOls = nullptr;
DeinitializeOls_t DeinitializeOls = nullptr;
ReadIoPortByte_t ReadIoPortByte = nullptr;
WriteIoPortByte_t WriteIoPortByte = nullptr;

#define EC_DATA_PORT 0x62
#define EC_CMD_PORT 0x66

bool WaitECReady() {
  int timeout = 100;
  while (timeout-- > 0) {
    if (ReadIoPortByte && (ReadIoPortByte(EC_CMD_PORT) & 0x02) == 0)
      return true;
    Sleep(1);
  }
  return false;
}

bool LoadWinRing0() {
#ifdef _WIN64
  g_hWinRing0 = LoadLibrary(L"WinRing0x64.dll");
#else
  g_hWinRing0 = LoadLibrary(L"WinRing0.dll");
#endif
  if (!g_hWinRing0) {
    MessageBox(nullptr, L_MSG_DLL_LOAD_FAIL[g_currentLanguage],
               L_MSG_ERROR[g_currentLanguage], MB_OK | MB_ICONERROR);
    return false;
  }
  InitializeOls = (InitializeOls_t)GetProcAddress(g_hWinRing0, "InitializeOls");
  DeinitializeOls =
      (DeinitializeOls_t)GetProcAddress(g_hWinRing0, "DeinitializeOls");
  ReadIoPortByte =
      (ReadIoPortByte_t)GetProcAddress(g_hWinRing0, "ReadIoPortByte");
  WriteIoPortByte =
      (WriteIoPortByte_t)GetProcAddress(g_hWinRing0, "WriteIoPortByte");
  if (!InitializeOls || !DeinitializeOls || !ReadIoPortByte ||
      !WriteIoPortByte) {
    MessageBox(nullptr, L_MSG_FUNC_LOAD_FAIL[g_currentLanguage],
               L_MSG_ERROR[g_currentLanguage], MB_OK | MB_ICONERROR);
    UnloadWinRing0();
    return false;
  }
  return true;
}

void UnloadWinRing0() {
  if (g_hWinRing0) {
    FreeLibrary(g_hWinRing0);
    g_hWinRing0 = nullptr;
  }
  InitializeOls = nullptr;
  DeinitializeOls = nullptr;
  ReadIoPortByte = nullptr;
  WriteIoPortByte = nullptr;
}

bool InitializeWinRing0() {
  if (!g_isXiaomiHardware) {
    g_winRing0Initialized = true;
    return true;
  }
  if (!LoadWinRing0())
    return false;
  if (!InitializeOls()) {
    MessageBox(nullptr, L_MSG_INIT_FAIL[g_currentLanguage],
               L_MSG_ERROR[g_currentLanguage], MB_OK | MB_ICONERROR);
    UnloadWinRing0();
    return false;
  }
  g_winRing0Initialized = true;
  return true;
}

void DeinitializeWinRing0() {
  if (!g_isXiaomiHardware) {
    g_winRing0Initialized = false;
    return;
  }
  if (g_winRing0Initialized && DeinitializeOls) {
    DeinitializeOls();
    g_winRing0Initialized = false;
  }
  UnloadWinRing0();
}

BYTE ReadEC(WORD address) {
  if (!g_isXiaomiHardware) {
    if (address == EC_BATTERY_CARE_ADDR) {
      return g_ecBatteryCareEnabled ? 0x01 : 0x00;
    }
    if (address == EC_BATTERY_LEVEL_ADDR) {
      return (BYTE)g_ecBatteryLevel;
    }
    if (address == EC_PERFORMANCE_MODE_ADDR) {
      if (g_selectedPerformanceMode == 0)
        return PERF_ECO_MODE;
      if (g_selectedPerformanceMode == 2)
        return PERF_FAST_MODE;
      return PERF_SMART_MODE;
    }
    return 0;
  }
  if (!g_winRing0Initialized || !ReadIoPortByte || !WriteIoPortByte)
    return 0;
  if (!WaitECReady())
    return 0;
  WriteIoPortByte(EC_CMD_PORT, 0x80);
  if (!WaitECReady())
    return 0;
  WriteIoPortByte(EC_DATA_PORT, (BYTE)address);
  if (!WaitECReady())
    return 0;
  return ReadIoPortByte(EC_DATA_PORT);
}

void WriteEC(WORD address, BYTE value) {
  if (!g_isXiaomiHardware) {
    if (address == EC_BATTERY_CARE_ADDR) {
      g_ecBatteryCareEnabled = (value & 0x01) != 0;
    }
    if (address == EC_BATTERY_LEVEL_ADDR) {
      g_ecBatteryLevel = value;
    }
    if (address == EC_PERFORMANCE_MODE_ADDR) {
      if (value == PERF_ECO_MODE)
        g_selectedPerformanceMode = 0;
      else if (value == PERF_FAST_MODE)
        g_selectedPerformanceMode = 2;
      else
        g_selectedPerformanceMode = 1;
    }
    return;
  }
  if (!g_winRing0Initialized || !ReadIoPortByte || !WriteIoPortByte)
    return;
  if (!WaitECReady())
    return;
  WriteIoPortByte(EC_CMD_PORT, 0x81);
  if (!WaitECReady())
    return;
  WriteIoPortByte(EC_DATA_PORT, (BYTE)address);
  if (!WaitECReady())
    return;
  WriteIoPortByte(EC_DATA_PORT, value);
}

void UpdateBatteryCareStatus() {
  BYTE ecValue = 0;
  BYTE level = 0;
  for (int i = 0; i < 3; i++) {
    ecValue = ReadEC(EC_BATTERY_CARE_ADDR);
    level = ReadEC(EC_BATTERY_LEVEL_ADDR);
    if (level > 0 && level <= 100)
      break;
    Sleep(10);
  }
  if (level == 0 || level > 100)
    level = 80;
  bool isEnabled = (ecValue & 0x01) != 0;
  g_ecBatteryCareEnabled = isEnabled;
  g_ecBatteryLevel = level;
  if (g_hWnd) {
    InvalidateRect(g_hWnd, nullptr, FALSE);
    if (g_hBatteryCareToggle)
      InvalidateRect(g_hBatteryCareToggle, nullptr, FALSE);
    if (g_hPreset50)
      InvalidateRect(g_hPreset50, nullptr, FALSE);
    if (g_hPreset60)
      InvalidateRect(g_hPreset60, nullptr, FALSE);
    if (g_hPreset70)
      InvalidateRect(g_hPreset70, nullptr, FALSE);
    if (g_hPreset80)
      InvalidateRect(g_hPreset80, nullptr, FALSE);
    if (g_hPreset90)
      InvalidateRect(g_hPreset90, nullptr, FALSE);
    if (g_hPreset100)
      InvalidateRect(g_hPreset100, nullptr, FALSE);
  }
}

void UpdatePerformanceMode() {
  BYTE mode = ReadEC(EC_PERFORMANCE_MODE_ADDR);
  int modeIndex = 1;
  switch (mode) {
  case PERF_ECO_MODE:
  case PERF_QUIET_MODE:
    modeIndex = 0;
    break;
  case PERF_SMART_MODE:
    modeIndex = 1;
    break;
  case PERF_FAST_MODE:
  case PERF_EXTREME_MODE:
    modeIndex = 2;
    break;
  }
  g_ecPerformanceMode = modeIndex;
  g_selectedPerformanceMode = modeIndex;
  UpdatePerfControls();
  if (g_hWnd)
    InvalidateRect(g_hWnd, nullptr, FALSE);
}

void UpdatePerfControls() {
  if (!g_hPerfDescLabel)
    return;
  const wchar_t *desc = L"";
  if (g_selectedPerformanceMode == 0) {
    desc = L_ECO_DESC[g_currentLanguage];
  } else if (g_selectedPerformanceMode == 1) {
    desc = L_SMART_DESC[g_currentLanguage];
  } else {
    desc = L_TURBO_DESC[g_currentLanguage];
  }
  SetWindowText(g_hPerfDescLabel, desc);
  if (g_hPerfBtnEco)
    InvalidateRect(g_hPerfBtnEco, nullptr, FALSE);
  if (g_hPerfBtnSmart)
    InvalidateRect(g_hPerfBtnSmart, nullptr, FALSE);
  if (g_hPerfBtnTurbo)
    InvalidateRect(g_hPerfBtnTurbo, nullptr, FALSE);
}

void SetBatteryCare(bool enable, int level) {
  if (level < 0)
    level = 0;
  if (level > 100)
    level = 100;

  if (enable) {
    // Read the current state of the care register
    BYTE currentValue = ReadEC(EC_BATTERY_CARE_ADDR);

    // 1. Temporarily disable battery care to reset the EC charging state machine
    WriteEC(EC_BATTERY_CARE_ADDR, currentValue & 0xFE);

    // 2. Write the new battery charge limit level
    WriteEC(EC_BATTERY_LEVEL_ADDR, (BYTE)level);

    // 3. Re-enable battery care to trigger EC level re-evaluation and resume charging
    WriteEC(EC_BATTERY_CARE_ADDR, currentValue | 0x01);
  } else {
    // Disable battery care completely
    BYTE currentValue = ReadEC(EC_BATTERY_CARE_ADDR);
    WriteEC(EC_BATTERY_CARE_ADDR, currentValue & 0xFE);
  }
}

void SetPerformanceMode(int mode) {
  BYTE modeValue = PERF_SMART_MODE;
  switch (mode) {
  case 0:
    modeValue = PERF_ECO_MODE;
    break;
  case 1:
    modeValue = PERF_SMART_MODE;
    break;
  case 2:
    modeValue = PERF_FAST_MODE;
    break;
  }
  WriteEC(EC_PERFORMANCE_MODE_ADDR, modeValue);
}

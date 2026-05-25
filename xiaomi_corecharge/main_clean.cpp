#include <windows.h>
#include "resource.h"
#include <batclass.h>
#include <commctrl.h>
#include <fstream>
#include <gdiplus.h>
#include <memory>
#include <objidl.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <string>
#include <strsafe.h>


#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "gdiplus.lib")

#pragma comment(                                                               \
    linker,                                                                    \
    "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Window class name and title
const wchar_t *CLASS_NAME = L"XiaomiCoreCharge";
const wchar_t *WINDOW_TITLE = L"Xiaomi CoreCharge";

// Control IDs
#define ID_BATTERY_CARE_TOGGLE 1001
#define ID_AI_SCHEDULING_TOGGLE 1002
#define ID_CHARGE_MAINTENANCE_TOGGLE 1003
#define ID_PERF_BTN_ECO 1030
#define ID_PERF_BTN_SMART 1031
#define ID_PERF_BTN_TURBO 1032
#define ID_MIN_BUTTON 1004
#define ID_CLOSE_BUTTON 1005
#define ID_REFRESH_BUTTON 1013
#define ID_EC_STATUS_BATTERY 1019
#define ID_EC_STATUS_PERF 1020

#define ID_PRESET_50 1023
#define ID_PRESET_60 1024
#define ID_PRESET_70 1025
#define ID_PRESET_80 1026
#define ID_PRESET_90 1027
#define ID_PRESET_100 1028

#define ID_TRAY_RETRY_TIMER 2
#define ID_RESET_TEXT_TIMER 3
#define ID_AUTO_REFRESH_TIMER 4

#define ID_HOTKEY_BATTERY 100
#define ID_HOTKEY_PERFORMANCE 101

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 2001
#define ID_TRAY_SHOW 2002
#define ID_TRAY_BATTERY_ENABLE 2003
#define ID_TRAY_BATTERY_DISABLE 2004
#define ID_TRAY_PERF_ECO 2005
#define ID_TRAY_PERF_SMART 2007
#define ID_TRAY_PERF_FAST 2008

// EC address definitions
#define EC_BATTERY_CARE_ADDR 0xA4
#define EC_BATTERY_LEVEL_ADDR 0xA7
#define EC_PERFORMANCE_MODE_ADDR 0x68

// Performance mode values
#define PERF_ECO_MODE 0x0A
#define PERF_QUIET_MODE 0x02
#define PERF_SMART_MODE 0x09
#define PERF_FAST_MODE 0x03
#define PERF_EXTREME_MODE 0x04

// Config file path
const wchar_t *CONFIG_FILE = L"xiaomi_corecharge_config.ini";

// WinRing0 function pointer type definitions
typedef BOOL(WINAPI *InitializeOls_t)();
typedef VOID(WINAPI *DeinitializeOls_t)();
typedef BYTE(WINAPI *ReadIoPortByte_t)(WORD port);
typedef VOID(WINAPI *WriteIoPortByte_t)(WORD port, BYTE value);

// Global variables
HWND g_hWnd = nullptr;
HWND g_hBatteryCareToggle = nullptr;
HWND g_hPerfBtnEco = nullptr;
HWND g_hPerfBtnSmart = nullptr;
HWND g_hPerfBtnTurbo = nullptr;
HWND g_hPerfDescLabel = nullptr;
HWND g_hBatteryStatusLabel = nullptr;
int g_selectedPerformanceMode = 1; // 0=Eco, 1=Smart, 2=Turbo
bool g_isXiaomiHardware = false;

enum Language { LANG_RU = 0, LANG_EN = 1, LANG_ZH = 2 };
Language g_currentLanguage = LANG_RU;
bool g_minimizeToTrayOnClose = true;
bool g_showSettings = false;

// Settings Controls and IDs
#define ID_GEAR_BUTTON 1100
#define ID_LANG_COMBO 1101
#define ID_AUTOSTART_CHECK 1102
#define ID_MINIMIZE_CHECK 1103
#define ID_SETTINGS_BACK_BTN 1104
#define ID_AUTOSTART_TEXT 1105
#define ID_MINIMIZE_TEXT 1106

HWND g_hGearButton = nullptr;
HWND g_hLangCombo = nullptr;
HWND g_hAutostartCheck = nullptr;
HWND g_hMinimizeCheck = nullptr;
HWND g_hSettingsBackBtn = nullptr;
HWND g_hAutostartText = nullptr;
HWND g_hMinimizeText = nullptr;

const wchar_t *L_AUTO_SAVE_NOTICE[] = {L"Настройки сохраняются автоматически",
                                       L"Settings are saved automatically",
                                       L"设置会自动保存"};

// Localization dictionaries
const wchar_t *L_BATTERY[] = {L"Батарея", L"Battery", L"电池"};
const wchar_t *L_CHARGING[] = {L"Зарядка...", L"Charging...", L"正在充电..."};
const wchar_t *L_PLUGGED_IN[] = {L"Питание от сети", L"Plugged in",
                                 L"已接通电源"};
const wchar_t *L_LEVEL_TEXT[] = {L"Уровень заряда %d%%", L"Charge level %d%%",
                                 L"电量 %d%%"};
const wchar_t *L_BATTERY_CHARGED[] = {L"Батарея заряжена (%d%%)",
                                      L"Battery charged (%d%%)",
                                      L"电池已充满 (%d%%)"};
const wchar_t *L_DISCHARGING_SUB[] = {L"Доступное время | Заряд %d%%",
                                      L"Remaining time | Charge %d%%",
                                      L"剩余可用时间 | 电量 %d%%"};
const wchar_t *L_DISCHARGING_TIME[] = {L"%dч %02dмин", L"%dh %02dm",
                                       L"%d小时 %02d分钟"};
const wchar_t *L_CARE_LIMIT_STATUS[] = {L"Ограничено лимитом %d%% | Заряд %d%%",
                                        L"Limited to %d%% | Charge %d%%",
                                        L"已限制在 %d%% | 电量 %d%%"};

const wchar_t *L_ECO[] = {L"Эко", L"Eco", L"节能"};
const wchar_t *L_SMART[] = {L"Умный", L"Smart", L"智能"};
const wchar_t *L_TURBO[] = {L"Турбо", L"Turbo", L"狂暴"};

const wchar_t *L_ECO_DESC[] = {
    L"Эко: снижает энергопотребление и продлевает время работы",
    L"Eco: reduces power consumption and extends battery life",
    L"节能：降低功耗并延长续航时间"};
const wchar_t *L_SMART_DESC[] = {
    L"Умный: автоматически регулирует производительность",
    L"Smart: automatically adjusts performance", L"智能：自动调节性能"};
const wchar_t *L_TURBO_DESC[] = {L"Турбо: максимальная производительность",
                                 L"Turbo: maximum performance",
                                 L"狂暴：释放最大性能"};

const wchar_t *L_CARE_TITLE[] = {L"Обслуживание зарядки", L"Battery Care",
                                 L"电池保护"};
const wchar_t *L_CARE_DESC[] = {L"В этом режиме аккумулятор не будет полностью "
                                L"заряжаться для продления срока его службы",
                                L"In this mode, the battery will not be fully "
                                L"charged to extend its lifespan",
                                L"此模式下电池不会完全充满，以延长其使用寿命"};

const wchar_t *L_SETTINGS_TITLE[] = {L"Настройки", L"Settings", L"设置"};
const wchar_t *L_LANG_LABEL[] = {L"Язык", L"Language", L"语言"};
const wchar_t *L_AUTOSTART_LABEL[] = {L"Запускать при старте системы",
                                      L"Start with Windows", L"开机自动启动"};
const wchar_t *L_MINCLOSE_LABEL[] = {L"Сворачивать в трей при закрытии",
                                     L"Minimize to tray on close",
                                     L"关闭时最小化到系统托盘"};
const wchar_t *L_BACK_BTN[] = {L"Назад", L"Back", L"返回"};

const wchar_t *L_OSD_TEXTS[][3] = {
    {L"Эко режим", L"Eco Mode", L"省电模式"},
    {L"Умный режим", L"Smart Mode", L"智能模式"},
    {L"Турбо режим", L"Turbo Mode", L"狂暴模式"},
    {L"Обслуживание зарядки: Вкл", L"Battery Care: ON", L"电池保护：开启"},
    {L"Обслуживание зарядки: Выкл", L"Battery Care: OFF", L"电池保护：关闭"}};

const wchar_t *L_TRAY_OPEN[] = {L"Открыть окно", L"Open window", L"打开窗口"};
const wchar_t *L_TRAY_EXIT[] = {L"Выход", L"Exit", L"退出"};
const wchar_t *L_TRAY_CARE_ON[] = {L"Включено", L"Enabled", L"已开启"};
const wchar_t *L_TRAY_CARE_OFF[] = {L"Отключено", L"Disabled", L"已关闭"};
const wchar_t *L_TRAY_MODE[] = {L"Режим", L"Mode", L"模式"};

const wchar_t *L_MSG_ERROR[] = {L"Ошибка", L"Error", L"错误"};
const wchar_t *L_MSG_DLL_LOAD_FAIL[] = {
    L"Не удалось загрузить библиотеку WinRing0.",
    L"Failed to load WinRing0 library.", L"无法加载 WinRing0 库。"};
const wchar_t *L_MSG_FUNC_LOAD_FAIL[] = {L"Ошибка загрузки функций WinRing0.",
                                         L"Failed to load WinRing0 functions.",
                                         L"加载 WinRing0 函数失败。"};
const wchar_t *L_MSG_INIT_FAIL[] = {
    L"Не удалось инициализировать WinRing0. Запустите с правами "
    L"администратора.",
    L"Failed to initialize WinRing0. Please run as administrator.",
    L"初始化 WinRing0 失败。请以管理员身份运行。"};
const wchar_t *L_MSG_REG_CLASS_FAIL[] = {
    L"Не удалось зарегистрировать класс окна",
    L"Failed to register window class", L"未能注册窗口类"};
const wchar_t *L_MSG_CREATE_WINDOW_FAIL[] = {
    L"Не удалось создать окно", L"Failed to create window", L"创建窗口失败"};
HWND g_hPreset50 = nullptr;
HWND g_hPreset60 = nullptr;
HWND g_hPreset70 = nullptr;
HWND g_hPreset80 = nullptr;
HWND g_hPreset90 = nullptr;
HWND g_hPreset100 = nullptr;
HWND g_hTitleLabel = nullptr;
NOTIFYICONDATA g_nid = {};
bool g_isMinimized = false;
bool g_isDragging = false;
POINT g_dragOffset = {};
HINSTANCE g_hInstance = nullptr;

// Fonts
HFONT g_hFontNormal = nullptr;
HFONT g_hFontTitle = nullptr;
HFONT g_hFontHeader = nullptr;
HFONT g_hFontSmall = nullptr;
HFONT g_hFontLarge = nullptr;
HFONT g_hFontMedium = nullptr;

// PC Manager Dark Theme Colors
HBRUSH g_hBgBrush = nullptr;            // Window background (#1a1a1a)
HBRUSH g_hCardBrush = nullptr;          // Card background (#252525)
HBRUSH g_hAccentBrush = nullptr;        // Accent Green (#22c55e)
HBRUSH g_hAccentHoverBrush = nullptr;   // Accent Green Hover (#16a34a)
HBRUSH g_hHoverBrush = nullptr;         // Grey hover (#333333)
HBRUSH g_hButtonBrush = nullptr;        // Button background (#2d2d2d)
HBRUSH g_hButtonHoverBrush = nullptr;   // Button hover (#404040)
HBRUSH g_hButtonPressedBrush = nullptr; // Button pressed (#1a1a1a)
HBRUSH g_hCloseHoverBrush = nullptr;    // Close hover (#ef4444)
HBRUSH g_hClosePressedBrush = nullptr;  // Close pressed (#dc2626)
HBRUSH g_hToggleOffBrush = nullptr;     // Toggle off (#404040)
HBRUSH g_hToggleOnBrush = nullptr;      // Toggle on (#22c55e)
HPEN g_hBorderPen = nullptr;            // Border (#333333)
HPEN g_hAccentPen = nullptr;            // Accent border (#22c55e)

// Color-coded mode elements (Eco=Green, Smart=Blue, Turbo=Orange)
HBRUSH g_hEcoBrush = nullptr;
HBRUSH g_hSmartBrush = nullptr;
HBRUSH g_hTurboBrush = nullptr;
HPEN g_hEcoPen = nullptr;
HPEN g_hSmartPen = nullptr;
HPEN g_hTurboPen = nullptr;

// EC dynamic state cache
bool g_ecBatteryCareEnabled = false;
int g_ecBatteryLevel = 57;
int g_ecPerformanceMode = 1;

UINT g_uTaskbarRestart = 0;

// OSD globals
HWND g_hwndOSD = nullptr;
int g_osdMode = 0;
int g_osdAlpha = 225;
HBITMAP g_hOsdBitmap = nullptr;
HDC g_osdMemDC = nullptr;
HBITMAP g_osdOldBitmap = nullptr;

#define TIMER_OSD_DISPLAY 100
#define TIMER_OSD_FADE 101

// WinRing0 related
HMODULE g_hWinRing0 = nullptr;
bool g_winRing0Initialized = false;
InitializeOls_t InitializeOls = nullptr;
DeinitializeOls_t DeinitializeOls = nullptr;
ReadIoPortByte_t ReadIoPortByte = nullptr;
WriteIoPortByte_t WriteIoPortByte = nullptr;

// Function declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ButtonSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam, UINT_PTR uIdSubclass,
                                    DWORD_PTR dwRefData);
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
void CreateTrayIcon();
void RemoveTrayIcon();
void ShowTrayMenu();
void ToggleMainWindow();
void CreateControls(HWND hwnd);
void DrawBackground(HDC hdc, RECT *rect);
void SaveConfig();
void LoadConfig();
void SetAutostart(bool enable);
bool GetAutostart();
void ToggleSettingsUI(bool show);
void UpdateLanguageUI();
void ShowOSD(int mode);
void RenderOSD(int mode, int alpha);
LRESULT CALLBACK OSDWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                               LPARAM lParam);

// Load WinRing0 DLL
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

bool IsXiaomiHardware() {
  HKEY hKey;
  bool isXiaomi = false;
  if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS",
                    0, KEY_READ, &hKey) == ERROR_SUCCESS) {
    wchar_t manufacturer[256] = {0};
    DWORD dwType = REG_SZ;
    DWORD dwSize = sizeof(manufacturer);
    if (RegQueryValueExW(hKey, L"SystemManufacturer", nullptr, &dwType,
                         (LPBYTE)manufacturer, &dwSize) == ERROR_SUCCESS) {
      std::wstring mfg(manufacturer);
      if (mfg.find(L"Xiaomi") != std::wstring::npos ||
          mfg.find(L"TIMI") != std::wstring::npos) {
        isXiaomi = true;
      }
    }
    RegCloseKey(hKey);
  }
  return isXiaomi;
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

// GetBatteryHealthPercentage removed as battery health status was deleted per
// user request.

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
  if (enable)
    WriteEC(EC_BATTERY_LEVEL_ADDR, (BYTE)level);
  BYTE currentValue = ReadEC(EC_BATTERY_CARE_ADDR);
  if (enable)
    currentValue |= 0x01;
  else
    currentValue &= 0xFE;
  WriteEC(EC_BATTERY_CARE_ADDR, currentValue);
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

void SaveConfig() {
  std::wofstream ofs(CONFIG_FILE);
  if (!ofs.is_open() || !ofs.good())
    return;
  ofs << L"battery_care=" << (g_ecBatteryCareEnabled ? 1 : 0) << std::endl;
  ofs << L"battery_level=" << g_ecBatteryLevel << std::endl;
  ofs << L"perf_mode=" << g_selectedPerformanceMode << std::endl;
  ofs << L"language=" << (int)g_currentLanguage << std::endl;
  ofs << L"autostart=" << (GetAutostart() ? 1 : 0) << std::endl;
  ofs << L"minimize_to_tray=" << (g_minimizeToTrayOnClose ? 1 : 0) << std::endl;
  ofs.close();
}

void LoadConfig() {
  std::wifstream ifs(CONFIG_FILE);
  if (!ifs)
    return;
  std::wstring line;
  while (std::getline(ifs, line)) {
    if (line.find(L"battery_care=") == 0)
      g_ecBatteryCareEnabled = std::stoi(line.substr(13)) != 0;
    else if (line.find(L"battery_level=") == 0)
      g_ecBatteryLevel = std::stoi(line.substr(14));
    else if (line.find(L"perf_mode=") == 0)
      g_selectedPerformanceMode = std::stoi(line.substr(10));
    else if (line.find(L"language=") == 0) {
      g_currentLanguage = (Language)std::stoi(line.substr(9));
      if (g_currentLanguage > LANG_ZH)
        g_currentLanguage = LANG_RU;
    } else if (line.find(L"autostart=") == 0) {
      bool autostart = std::stoi(line.substr(10)) != 0;
      SetAutostart(autostart);
    } else if (line.find(L"minimize_to_tray=") == 0) {
      g_minimizeToTrayOnClose = std::stoi(line.substr(17)) != 0;
    }
  }
}

void SetAutostart(bool enable) {
  HKEY hKey;
  if (RegOpenKeyExW(HKEY_CURRENT_USER,
                    L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0,
                    KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
    if (enable) {
      wchar_t exePath[MAX_PATH];
      GetModuleFileNameW(nullptr, exePath, MAX_PATH);
      RegSetValueExW(hKey, L"XiaomiCoreCharge", 0, REG_SZ, (BYTE *)exePath,
                     (DWORD)(wcslen(exePath) + 1) * sizeof(wchar_t));
    } else {
      RegDeleteValueW(hKey, L"XiaomiCoreCharge");
    }
    RegCloseKey(hKey);
  }
}

bool GetAutostart() {
  HKEY hKey;
  bool enabled = false;
  if (RegOpenKeyExW(HKEY_CURRENT_USER,
                    L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0,
                    KEY_READ, &hKey) == ERROR_SUCCESS) {
    if (RegQueryValueExW(hKey, L"XiaomiCoreCharge", nullptr, nullptr, nullptr,
                         nullptr) == ERROR_SUCCESS) {
      enabled = true;
    }
    RegCloseKey(hKey);
  }
  return enabled;
}

void ToggleSettingsUI(bool show) {
  int showCmd = show ? SW_SHOW : SW_HIDE;
  int hideCmd = show ? SW_HIDE : SW_SHOW;

  ShowWindow(g_hPerfBtnEco, hideCmd);
  ShowWindow(g_hPerfBtnSmart, hideCmd);
  ShowWindow(g_hPerfBtnTurbo, hideCmd);
  ShowWindow(g_hBatteryCareToggle, hideCmd);
  ShowWindow(g_hPreset50, hideCmd);
  ShowWindow(g_hPreset60, hideCmd);
  ShowWindow(g_hPreset70, hideCmd);
  ShowWindow(g_hPreset80, hideCmd);
  ShowWindow(g_hPreset90, hideCmd);
  ShowWindow(g_hPreset100, hideCmd);
  ShowWindow(g_hPerfDescLabel, hideCmd);

  // Settings controls
  ShowWindow(g_hLangCombo, showCmd);
  ShowWindow(g_hAutostartCheck, showCmd);
  ShowWindow(g_hMinimizeCheck, showCmd);
  ShowWindow(g_hSettingsBackBtn, showCmd);
  ShowWindow(g_hAutostartText, showCmd);
  ShowWindow(g_hMinimizeText, showCmd);

  // Keep checkbox states fresh when showing settings
  if (show) {
    SendMessage(g_hAutostartCheck, BM_SETCHECK,
                GetAutostart() ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(g_hMinimizeCheck, BM_SETCHECK,
                g_minimizeToTrayOnClose ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(g_hLangCombo, CB_SETCURSEL, g_currentLanguage, 0);
  }
}

void UpdateLanguageUI() {
  if (!g_hWnd)
    return;
  // Update Title
  SetWindowText(g_hTitleLabel, L_BATTERY[g_currentLanguage]);

  // Update Performance buttons
  SetWindowText(g_hPerfBtnEco, L_ECO[g_currentLanguage]);
  SetWindowText(g_hPerfBtnSmart, L_SMART[g_currentLanguage]);
  SetWindowText(g_hPerfBtnTurbo, L_TURBO[g_currentLanguage]);

  // Update description
  UpdatePerfControls();

  // Update settings labels if settings window is active
  if (g_hAutostartText) {
    SetWindowText(g_hAutostartText, L_AUTOSTART_LABEL[g_currentLanguage]);
  }
  if (g_hMinimizeText) {
    SetWindowText(g_hMinimizeText, L_MINCLOSE_LABEL[g_currentLanguage]);
  }
  if (g_hSettingsBackBtn) {
    SetWindowText(g_hSettingsBackBtn, L_BACK_BTN[g_currentLanguage]);
  }

  InvalidateRect(g_hWnd, nullptr, FALSE);
}

void RenderOSD(int mode, int alpha) {
  if (!g_hwndOSD)
    return;

  const int width = 240;
  const int height = 240;

  HDC hdcScreen = GetDC(nullptr);
  if (!g_osdMemDC) {
    g_osdMemDC = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    g_hOsdBitmap =
        CreateDIBSection(g_osdMemDC, &bmi, DIB_RGB_COLORS, nullptr, nullptr, 0);
    g_osdOldBitmap = (HBITMAP)SelectObject(g_osdMemDC, g_hOsdBitmap);
  }

  Gdiplus::Graphics graphics(g_osdMemDC);
  graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
  graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

  // Clear with transparent
  graphics.Clear(Gdiplus::Color(0, 0, 0, 0));

  // Determine colors based on mode (gorgeous premium modern gradient overlay as
  // requested)
  Gdiplus::Color colorStart, colorEnd;
  if (mode == 0) { // Eco
    colorStart = Gdiplus::Color(
        225, 12, 85,
        45); // Deep magenta-purple gradients as in PC Manager screenshots
    colorEnd = Gdiplus::Color(225, 4, 30, 20);
  } else if (mode == 1) {                         // Smart
    colorStart = Gdiplus::Color(225, 45, 15, 90); // Deep violet gradient
    colorEnd = Gdiplus::Color(225, 20, 5, 50);
  } else if (mode == 2) { // Turbo
    colorStart =
        Gdiplus::Color(225, 160, 30, 10); // Deep vibrant orange-red gradient
    colorEnd = Gdiplus::Color(225, 90, 10, 5);
  } else if (mode == 3) {                          // Battery Care ON
    colorStart = Gdiplus::Color(225, 16, 185, 90); // Emerald green gradient
    colorEnd = Gdiplus::Color(225, 10, 100, 50);
  } else { // Battery Care OFF
    colorStart =
        Gdiplus::Color(225, 70, 70, 70); // Dark grey / charcoal card gradient
    colorEnd = Gdiplus::Color(225, 35, 35, 35);
  }

  Gdiplus::LinearGradientBrush cardBrush(
      Gdiplus::Rect(0, 0, width, height), colorStart, colorEnd,
      Gdiplus::LinearGradientModeForwardDiagonal);

  // Draw rounded card
  Gdiplus::GraphicsPath path;
  float r = 28.0f; // Rounded corner radius
  float pad = 12.0f;
  path.AddArc(pad, pad, r * 2, r * 2, 180.0f, 90.0f);
  path.AddArc(width - pad - r * 2, pad, r * 2, r * 2, 270.0f, 90.0f);
  path.AddArc(width - pad - r * 2, height - pad - r * 2, r * 2, r * 2, 0.0f,
              90.0f);
  path.AddArc(pad, height - pad - r * 2, r * 2, r * 2, 90.0f, 90.0f);
  path.CloseFigure();

  graphics.FillPath(&cardBrush, &path);

  // Draw vector icons
  Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 255, 255, 255));

  // Eco Mode: Battery with leaf
  if (mode == 0) {
    Gdiplus::Pen batteryPen(Gdiplus::Color(255, 255, 255, 255), 2.5f);
    batteryPen.SetLineJoin(Gdiplus::LineJoinRound);

    // Draw battery body
    Gdiplus::RectF batRect(80.0f, 75.0f, 70.0f, 40.0f);
    graphics.DrawRectangle(&batteryPen, batRect);

    // Draw battery cap
    Gdiplus::RectF capRect(150.0f, 85.0f, 6.0f, 20.0f);
    graphics.FillRectangle(&whiteBrush, capRect);

    // Draw leaf inside the battery
    Gdiplus::GraphicsPath leafPath;
    leafPath.AddBezier(92.0f, 102.0f, 92.0f, 83.0f, 118.0f, 83.0f, 138.0f,
                       95.0f);
    leafPath.AddBezier(138.0f, 95.0f, 128.0f, 112.0f, 108.0f, 112.0f, 92.0f,
                       102.0f);
    graphics.FillPath(&whiteBrush, &leafPath);

    // Draw leaf vein
    Gdiplus::Pen veinPen(Gdiplus::Color(220, 12, 85, 45), 1.5f);
    graphics.DrawLine(&veinPen, 92.0f, 102.0f, 126.0f, 91.0f);
  }
  // Smart Mode: Speedometer with "AUTO"
  else if (mode == 1) {
    Gdiplus::Pen gaugePen(Gdiplus::Color(255, 255, 255, 255), 2.5f);
    graphics.DrawArc(&gaugePen, 70.0f, 55.0f, 100.0f, 100.0f, 140.0f, 260.0f);

    // Draw tick marks radiating outwards
    float cx = 120.0f, cy = 105.0f;
    for (int i = 0; i <= 6; i++) {
      float angle = (140.0f + i * 260.0f / 6.0f) * 3.14159f / 180.0f;
      float rInner = 43.0f;
      float rOuter = 48.0f;
      graphics.DrawLine(&gaugePen, cx + rInner * cosf(angle),
                        cy + rInner * sinf(angle), cx + rOuter * cosf(angle),
                        cy + rOuter * sinf(angle));
    }

    // Draw "AUTO" text
    Gdiplus::Font autoFont(L"Segoe UI", 13.0f, Gdiplus::FontStyleBold);
    Gdiplus::StringFormat stringFormat;
    stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
    stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    graphics.DrawString(L"AUTO", -1, &autoFont, Gdiplus::PointF(cx, cy),
                        &stringFormat, &whiteBrush);
  }
  // Turbo Mode: Lightning bolt
  else if (mode == 2) {
    Gdiplus::GraphicsPath boltPath;
    Gdiplus::PointF boltPts[] = {{125.0f, 45.0f},  {95.0f, 100.0f},
                                 {118.0f, 100.0f}, {115.0f, 155.0f},
                                 {145.0f, 100.0f}, {122.0f, 100.0f}};
    boltPath.AddPolygon(boltPts, 6);
    graphics.FillPath(&whiteBrush, &boltPath);
  }
  // Battery Care ON: Battery with checkmark
  else if (mode == 3) {
    Gdiplus::Pen batteryPen(Gdiplus::Color(255, 255, 255, 255), 2.5f);
    batteryPen.SetLineJoin(Gdiplus::LineJoinRound);

    // Draw battery body
    Gdiplus::RectF batRect(80.0f, 75.0f, 70.0f, 40.0f);
    graphics.DrawRectangle(&batteryPen, batRect);

    // Draw battery cap
    Gdiplus::RectF capRect(150.0f, 85.0f, 6.0f, 20.0f);
    graphics.FillRectangle(&whiteBrush, capRect);

    // Draw Checkmark
    Gdiplus::Pen checkPen(Gdiplus::Color(255, 255, 255, 255), 4.0f);
    checkPen.SetLineCap(Gdiplus::LineCapRound, Gdiplus::LineCapRound,
                        Gdiplus::DashCapRound);
    checkPen.SetLineJoin(Gdiplus::LineJoinRound);

    Gdiplus::PointF chkPts[] = {
        {98.0f, 95.0f}, {109.0f, 106.0f}, {132.0f, 83.0f}};
    graphics.DrawLines(&checkPen, chkPts, 3);
  }
  // Battery Care OFF: Battery with red cancel slash
  else {
    Gdiplus::Pen batteryPen(Gdiplus::Color(255, 255, 255, 255), 2.5f);
    batteryPen.SetLineJoin(Gdiplus::LineJoinRound);

    // Draw battery body
    Gdiplus::RectF batRect(80.0f, 75.0f, 70.0f, 40.0f);
    graphics.DrawRectangle(&batteryPen, batRect);

    // Draw battery cap
    Gdiplus::RectF capRect(150.0f, 85.0f, 6.0f, 20.0f);
    graphics.FillRectangle(&whiteBrush, capRect);

    // Draw Red diagonal cancel line
    Gdiplus::Pen redPen(Gdiplus::Color(255, 239, 68, 68), 4.0f);
    redPen.SetLineCap(Gdiplus::LineCapRound, Gdiplus::LineCapRound,
                      Gdiplus::DashCapRound);
    graphics.DrawLine(&redPen, 72.0f, 70.0f, 158.0f, 120.0f);
  }

  // Draw Mode Label
  Gdiplus::Font textFont(L"Segoe UI", 9.0f, Gdiplus::FontStyleBold);
  Gdiplus::StringFormat stringFormat;
  stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
  stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);

  const wchar_t *osdText = L_OSD_TEXTS[mode][g_currentLanguage];
  graphics.DrawString(osdText, -1, &textFont,
                      Gdiplus::PointF((float)width / 2, 195.0f), &stringFormat,
                      &whiteBrush);

  // Position in center of screen
  int screenW = GetSystemMetrics(SM_CXSCREEN);
  int screenH = GetSystemMetrics(SM_CYSCREEN);
  POINT ptPos = {(screenW - width) / 2, (screenH - height) / 2};
  SIZE size = {width, height};
  POINT ptSrc = {0, 0};

  BLENDFUNCTION blend = {};
  blend.BlendOp = AC_SRC_OVER;
  blend.SourceConstantAlpha = (BYTE)alpha;
  blend.AlphaFormat = AC_SRC_ALPHA;

  UpdateLayeredWindow(g_hwndOSD, hdcScreen, &ptPos, &size, g_osdMemDC, &ptSrc,
                      0, &blend, ULW_ALPHA);
  ReleaseDC(nullptr, hdcScreen);
}

LRESULT CALLBACK OSDWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                               LPARAM lParam) {
  switch (uMsg) {
  case WM_TIMER:
    if (wParam == TIMER_OSD_DISPLAY) {
      KillTimer(hwnd, TIMER_OSD_DISPLAY);
      SetTimer(hwnd, TIMER_OSD_FADE, 15, nullptr);
    } else if (wParam == TIMER_OSD_FADE) {
      g_osdAlpha -= 15;
      if (g_osdAlpha <= 0) {
        g_osdAlpha = 0;
        KillTimer(hwnd, TIMER_OSD_FADE);
        ShowWindow(hwnd, SW_HIDE);
      } else {
        RenderOSD(g_osdMode, g_osdAlpha);
      }
    }
    break;
  case WM_DESTROY:
    if (g_osdMemDC) {
      SelectObject(g_osdMemDC, g_osdOldBitmap);
      DeleteObject(g_hOsdBitmap);
      DeleteDC(g_osdMemDC);
      g_osdMemDC = nullptr;
      g_hOsdBitmap = nullptr;
      g_osdOldBitmap = nullptr;
    }
    break;
  default:
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
  return 0;
}

void ShowOSD(int mode) {
  if (!g_hwndOSD) {
    WNDCLASS wcOSD = {};
    wcOSD.lpfnWndProc = OSDWindowProc;
    wcOSD.hInstance = g_hInstance;
    wcOSD.lpszClassName = L"XiaomiCoreChargeOSD";
    wcOSD.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClass(&wcOSD);

    g_hwndOSD = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        L"XiaomiCoreChargeOSD", L"", WS_POPUP, 0, 0, 240, 240, nullptr, nullptr,
        g_hInstance, nullptr);
  }

  KillTimer(g_hwndOSD, TIMER_OSD_DISPLAY);
  KillTimer(g_hwndOSD, TIMER_OSD_FADE);

  g_osdMode = mode;
  g_osdAlpha = 225; // 225 out of 255 alpha for a premium glassmorphic feel

  RenderOSD(mode, g_osdAlpha);
  ShowWindow(g_hwndOSD, SW_SHOWNOACTIVATE);

  SetTimer(g_hwndOSD, TIMER_OSD_DISPLAY, 1200, nullptr);
}

void CreateTrayIcon() {
  ZeroMemory(&g_nid, sizeof(NOTIFYICONDATA));
  g_nid.cbSize = sizeof(NOTIFYICONDATA);
  g_nid.hWnd = g_hWnd;
  g_nid.uID = 1;
  g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
  g_nid.uCallbackMessage = WM_TRAYICON;
  g_nid.hIcon = (HICON)LoadImage(g_hInstance, MAKEINTRESOURCE(IDI_TRAY_ICON), IMAGE_ICON,
                                 GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
                                 LR_DEFAULTCOLOR | LR_SHARED);
  if (!g_nid.hIcon)
    g_nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
  wcscpy_s(g_nid.szTip, sizeof(g_nid.szTip) / sizeof(wchar_t),
           L"Xiaomi CoreCharge");
  if (!Shell_NotifyIcon(NIM_ADD, &g_nid)) {
    SetTimer(g_hWnd, ID_TRAY_RETRY_TIMER, 5000, nullptr);
  } else {
    KillTimer(g_hWnd, ID_TRAY_RETRY_TIMER);
  }
}

void RemoveTrayIcon() { Shell_NotifyIcon(NIM_DELETE, &g_nid); }

void ShowTrayMenu() {
  POINT pt;
  GetCursorPos(&pt);
  HMENU hMenu = CreatePopupMenu();
  AppendMenu(hMenu, MF_STRING, ID_TRAY_SHOW, L_TRAY_OPEN[g_currentLanguage]);
  AppendMenu(hMenu, MF_SEPARATOR, 0, nullptr);

  HMENU hBatteryMenu = CreatePopupMenu();
  AppendMenu(hBatteryMenu,
             MF_STRING | (g_ecBatteryCareEnabled ? MF_CHECKED : MF_UNCHECKED),
             ID_TRAY_BATTERY_ENABLE, L_TRAY_CARE_ON[g_currentLanguage]);
  AppendMenu(hBatteryMenu,
             MF_STRING | (!g_ecBatteryCareEnabled ? MF_CHECKED : MF_UNCHECKED),
             ID_TRAY_BATTERY_DISABLE, L_TRAY_CARE_OFF[g_currentLanguage]);
  AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hBatteryMenu,
             L_CARE_TITLE[g_currentLanguage]);

  HMENU hPerfMenu = CreatePopupMenu();
  AppendMenu(hPerfMenu,
             MF_STRING |
                 (g_selectedPerformanceMode == 0 ? MF_CHECKED : MF_UNCHECKED),
             ID_TRAY_PERF_ECO, L_ECO[g_currentLanguage]);
  AppendMenu(hPerfMenu,
             MF_STRING |
                 (g_selectedPerformanceMode == 1 ? MF_CHECKED : MF_UNCHECKED),
             ID_TRAY_PERF_SMART, L_SMART[g_currentLanguage]);
  AppendMenu(hPerfMenu,
             MF_STRING |
                 (g_selectedPerformanceMode == 2 ? MF_CHECKED : MF_UNCHECKED),
             ID_TRAY_PERF_FAST, L_TURBO[g_currentLanguage]);
  AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hPerfMenu,
             L_TRAY_MODE[g_currentLanguage]);

  AppendMenu(hMenu, MF_SEPARATOR, 0, nullptr);
  AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, L_TRAY_EXIT[g_currentLanguage]);

  SetForegroundWindow(g_hWnd);
  TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, g_hWnd, nullptr);
  DestroyMenu(hMenu);
}

void ToggleMainWindow() {
  if (g_isMinimized || !IsWindowVisible(g_hWnd)) {
    ShowWindow(g_hWnd, SW_SHOW);
    ShowWindow(g_hWnd, SW_RESTORE);
    SetForegroundWindow(g_hWnd);
    BringWindowToTop(g_hWnd);
    g_isMinimized = false;
  } else {
    ShowWindow(g_hWnd, SW_HIDE);
    g_isMinimized = true;
  }
}

// Create controls - PC Manager style layout
void CreateControls(HWND hwnd) {
  // Title bar with app name
  g_hTitleLabel = CreateWindow(
      L"STATIC", L"Батарея", WS_VISIBLE | WS_CHILD | SS_LEFT, 32, 60, 300, 36,
      hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
  SendMessage(g_hTitleLabel, WM_SETFONT, (WPARAM)g_hFontLarge, TRUE);

  // Performance mode description label
  g_hPerfDescLabel = CreateWindow(
      L"STATIC", L"Умный: автоматически регулирует производительность",
      WS_VISIBLE | WS_CHILD | SS_LEFT, 380, 215, 380, 20, hwnd, nullptr,
      GetModuleHandle(nullptr), nullptr);
  SendMessage(g_hPerfDescLabel, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

  // Performance mode buttons (Eco, Smart, Turbo) - right side
  g_hPerfBtnEco = CreateWindow(
      L"BUTTON", L"Эко", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
      380, 115, 110, 90, hwnd, (HMENU)ID_PERF_BTN_ECO, GetModuleHandle(nullptr),
      nullptr);
  SendMessage(g_hPerfBtnEco, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

  g_hPerfBtnSmart = CreateWindow(
      L"BUTTON", L"Умный", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
      500, 115, 110, 90, hwnd, (HMENU)ID_PERF_BTN_SMART,
      GetModuleHandle(nullptr), nullptr);
  SendMessage(g_hPerfBtnSmart, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

  g_hPerfBtnTurbo = CreateWindow(
      L"BUTTON", L"Турбо", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
      620, 115, 110, 90, hwnd, (HMENU)ID_PERF_BTN_TURBO,
      GetModuleHandle(nullptr), nullptr);
  SendMessage(g_hPerfBtnTurbo, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

  // Toggle buttons for settings (shifted up from y=370 to y=285)
  g_hBatteryCareToggle = CreateWindow(
      L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 700,
      285, 50, 26, hwnd, (HMENU)ID_BATTERY_CARE_TOGGLE,
      GetModuleHandle(nullptr), nullptr);

  // Preset buttons (pill-shaped) under Card 2 description (shifted down to
  // y=335 for comfortable spacing)
  g_hPreset50 = CreateWindow(
      L"BUTTON", L"50%", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
      50, 335, 65, 30, hwnd, (HMENU)ID_PRESET_50, GetModuleHandle(nullptr),
      nullptr);
  g_hPreset60 = CreateWindow(
      L"BUTTON", L"60%", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
      125, 335, 65, 30, hwnd, (HMENU)ID_PRESET_60, GetModuleHandle(nullptr),
      nullptr);
  g_hPreset70 = CreateWindow(
      L"BUTTON", L"70%", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
      200, 335, 65, 30, hwnd, (HMENU)ID_PRESET_70, GetModuleHandle(nullptr),
      nullptr);
  g_hPreset80 = CreateWindow(
      L"BUTTON", L"80%", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
      275, 335, 65, 30, hwnd, (HMENU)ID_PRESET_80, GetModuleHandle(nullptr),
      nullptr);
  g_hPreset90 = CreateWindow(
      L"BUTTON", L"90%", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
      350, 335, 65, 30, hwnd, (HMENU)ID_PRESET_90, GetModuleHandle(nullptr),
      nullptr);
  g_hPreset100 = CreateWindow(
      L"BUTTON", L"100%", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
      425, 335, 65, 30, hwnd, (HMENU)ID_PRESET_100, GetModuleHandle(nullptr),
      nullptr);

  SendMessage(g_hPreset50, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
  SendMessage(g_hPreset60, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
  SendMessage(g_hPreset70, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
  SendMessage(g_hPreset80, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
  SendMessage(g_hPreset90, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
  SendMessage(g_hPreset100, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

  // Window control buttons (gear, minimize, close)
  g_hGearButton = CreateWindow(
      L"BUTTON", L"", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 680,
      10, 40, 32, hwnd, (HMENU)ID_GEAR_BUTTON, GetModuleHandle(nullptr),
      nullptr);

  HWND hMin = CreateWindow(L"BUTTON", L"\u2014",
                           WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
                           720, 10, 40, 32, hwnd, (HMENU)ID_MIN_BUTTON,
                           GetModuleHandle(nullptr), nullptr);
  HWND hClose = CreateWindow(
      L"BUTTON", L"\u00D7",
      WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 760, 10, 40, 32,
      hwnd, (HMENU)ID_CLOSE_BUTTON, GetModuleHandle(nullptr), nullptr);

  // Settings controls (initially hidden, CBS_DROPDOWNLIST style)
  g_hLangCombo = CreateWindow(
      L"COMBOBOX", L"", WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL, 160, 180, 180,
      150, hwnd, (HMENU)ID_LANG_COMBO, GetModuleHandle(nullptr), nullptr);
  SendMessage(g_hLangCombo, CB_ADDSTRING, 0, (LPARAM)L"Русский");
  SendMessage(g_hLangCombo, CB_ADDSTRING, 0, (LPARAM)L"English");
  SendMessage(g_hLangCombo, CB_ADDSTRING, 0, (LPARAM)L"中国");
  SendMessage(g_hLangCombo, CB_SETCURSEL, g_currentLanguage, 0);
  SendMessage(g_hLangCombo, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

  g_hAutostartCheck = CreateWindow(
      L"BUTTON", L"", WS_CHILD | BS_AUTOCHECKBOX, 50, 230, 18, 18, hwnd,
      (HMENU)ID_AUTOSTART_CHECK, GetModuleHandle(nullptr), nullptr);

  g_hAutostartText =
      CreateWindow(L"STATIC", L"Запускать при старте системы",
                   WS_CHILD | SS_LEFT | SS_NOTIFY, 76, 229, 600, 20, hwnd,
                   (HMENU)ID_AUTOSTART_TEXT, GetModuleHandle(nullptr), nullptr);
  SendMessage(g_hAutostartText, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

  g_hMinimizeCheck = CreateWindow(
      L"BUTTON", L"", WS_CHILD | BS_AUTOCHECKBOX, 50, 275, 18, 18, hwnd,
      (HMENU)ID_MINIMIZE_CHECK, GetModuleHandle(nullptr), nullptr);

  g_hMinimizeText =
      CreateWindow(L"STATIC", L"Сворачивать в трей при закрытии программы",
                   WS_CHILD | SS_LEFT | SS_NOTIFY, 76, 274, 600, 20, hwnd,
                   (HMENU)ID_MINIMIZE_TEXT, GetModuleHandle(nullptr), nullptr);
  SendMessage(g_hMinimizeText, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

  g_hSettingsBackBtn =
      CreateWindow(L"BUTTON", L"Назад", WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
                   50, 330, 110, 35, hwnd, (HMENU)ID_SETTINGS_BACK_BTN,
                   GetModuleHandle(nullptr), nullptr);
  SendMessage(g_hSettingsBackBtn, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);

  // Apply subclass for hover effects
  SetWindowSubclass(g_hBatteryCareToggle, ButtonSubclassProc, 0, 0);
  SetWindowSubclass(g_hPreset50, ButtonSubclassProc, 0, 0);
  SetWindowSubclass(g_hPreset60, ButtonSubclassProc, 0, 0);
  SetWindowSubclass(g_hPreset70, ButtonSubclassProc, 0, 0);
  SetWindowSubclass(g_hPreset80, ButtonSubclassProc, 0, 0);
  SetWindowSubclass(g_hPreset90, ButtonSubclassProc, 0, 0);
  SetWindowSubclass(g_hPreset100, ButtonSubclassProc, 0, 0);
  SetWindowSubclass(g_hPerfBtnEco, ButtonSubclassProc, 0, 0);
  SetWindowSubclass(g_hPerfBtnSmart, ButtonSubclassProc, 0, 0);
  SetWindowSubclass(g_hPerfBtnTurbo, ButtonSubclassProc, 0, 0);
  SetWindowSubclass(hMin, ButtonSubclassProc, 0, 0);
  SetWindowSubclass(hClose, ButtonSubclassProc, 0, 0);
  SetWindowSubclass(g_hGearButton, ButtonSubclassProc, 0, 0);
  SetWindowSubclass(g_hSettingsBackBtn, ButtonSubclassProc, 0, 0);
}

LRESULT CALLBACK ButtonSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam, UINT_PTR uIdSubclass,
                                    DWORD_PTR dwRefData) {
  switch (uMsg) {
  case WM_MOUSEMOVE: {
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(TRACKMOUSEEVENT);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = hWnd;
    TrackMouseEvent(&tme);
    if (!GetPropW(hWnd, L"Hover")) {
      SetPropW(hWnd, L"Hover", (HANDLE)TRUE);
      InvalidateRect(hWnd, nullptr, FALSE);
    }
    break;
  }
  case WM_MOUSELEAVE:
    RemovePropW(hWnd, L"Hover");
    InvalidateRect(hWnd, nullptr, FALSE);
    break;
  case WM_DESTROY:
    RemovePropW(hWnd, L"Hover");
    break;
  }
  return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Helper: Draw rounded rectangle
void DrawRoundedRect(HDC hdc, int x, int y, int w, int h, int r, HBRUSH brush,
                     HPEN pen) {
  HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
  HPEN oldPen = (HPEN)SelectObject(hdc, pen ? pen : GetStockObject(NULL_PEN));
  RoundRect(hdc, x, y, x + w, y + h, r, r);
  SelectObject(hdc, oldBrush);
  SelectObject(hdc, oldPen);
}

// Helper: Draw toggle switch
void DrawToggle(HDC hdc, int x, int y, bool isOn, bool isHovered) {
  int w = 50, h = 26, r = 13;

  // Track background
  HBRUSH trackBrush = isOn ? g_hAccentBrush : g_hToggleOffBrush;
  if (isHovered) {
    trackBrush = isOn ? g_hAccentHoverBrush : g_hHoverBrush;
  }

  HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, trackBrush);
  HPEN oldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));
  RoundRect(hdc, x, y, x + w, y + h, r * 2, r * 2);

  // Thumb (circle)
  int thumbX = isOn ? x + w - h + 3 : x + 3;
  int thumbY = y + 3;
  int thumbSize = h - 6;

  SelectObject(hdc, GetStockObject(WHITE_BRUSH));
  Ellipse(hdc, thumbX, thumbY, thumbX + thumbSize, thumbY + thumbSize);

  SelectObject(hdc, oldBrush);
  SelectObject(hdc, oldPen);
}

void DrawBackground(HDC hdc, RECT *rect) {
  // Fill background
  FillRect(hdc, rect, g_hBgBrush);

  // Draw window border
  HPEN hOldPen = (HPEN)SelectObject(hdc, g_hBorderPen);
  HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
  Rectangle(hdc, 0, 0, rect->right, rect->bottom);

  // Title bar area - "PC Manager" text style
  SetBkMode(hdc, TRANSPARENT);
  SetTextColor(hdc, RGB(255, 255, 255));

  // App icon placeholder (lightning bolt in orange square)
  HBRUSH hLogoBrush = CreateSolidBrush(RGB(255, 103, 0)); // Orange
  HBRUSH hOldBrushLogo = (HBRUSH)SelectObject(hdc, hLogoBrush);
  SelectObject(hdc, GetStockObject(NULL_PEN));
  RoundRect(hdc, 25, 12, 50, 37, 6, 6);
  SelectObject(hdc, hOldBrushLogo); // Restore brush immediately
  DeleteObject(hLogoBrush);

  // Lightning bolt icon
  POINT bolt[6] = {{38, 15}, {32, 24}, {36, 24}, {36, 32}, {42, 22}, {38, 22}};
  SelectObject(hdc, GetStockObject(WHITE_BRUSH));
  Polygon(hdc, bolt, 6);

  // App title
  HFONT hOldFont = (HFONT)SelectObject(hdc, g_hFontTitle);
  RECT titleRc = {60, 14, 300, 40};
  DrawTextW(hdc, L"Xiaomi CoreCharge", -1, &titleRc,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);

  // Version text
  SelectObject(hdc, g_hFontSmall);
  SetTextColor(hdc, RGB(140, 140, 140));
  RECT verRc = {230, 18, 300, 36};
  DrawTextW(hdc, L"v1.0", -1, &verRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

  // ========== SETTINGS CARDS (FULL OVERLAY IF ACTIVE) ==========
  if (g_showSettings) {
    SelectObject(hdc, g_hCardBrush);
    RoundRect(hdc, 32, 115, 760, 380, 12, 12);

    // Settings Title
    SetTextColor(hdc, RGB(255, 255, 255));
    SelectObject(hdc, g_hFontHeader);
    RECT settingsTitleRc = {50, 135, 400, 160};
    DrawTextW(hdc, L_SETTINGS_TITLE[g_currentLanguage], -1, &settingsTitleRc,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    // Language Label
    SetTextColor(hdc, RGB(170, 170, 170));
    SelectObject(hdc, g_hFontNormal);
    RECT langLabelRc = {50, 185, 150, 215};
    DrawTextW(hdc, L_LANG_LABEL[g_currentLanguage], -1, &langLabelRc,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    // Auto-Save Notice next to Back button
    SetTextColor(hdc, RGB(130, 130, 130));
    SelectObject(hdc, g_hFontNormal);
    RECT noticeRc = {180, 330, 700, 365};
    DrawTextW(hdc, L_AUTO_SAVE_NOTICE[g_currentLanguage], -1, &noticeRc,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, hOldFont);
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    return;
  }

  // ========== BATTERY STATUS CARD (Green gradient like PC Manager) ==========
  // Create gradient brush effect with solid color
  HBRUSH hBatteryCardBrush = CreateSolidBrush(RGB(34, 197, 94)); // Green
  HBRUSH hOldBrush2 = (HBRUSH)SelectObject(
      hdc, hBatteryCardBrush); // Save old brush for safe deletion
  SelectObject(hdc, GetStockObject(NULL_PEN));
  RoundRect(hdc, 32, 115, 350, 205, 16, 16);

  // Battery time remaining (large text)
  SetTextColor(hdc, RGB(255, 255, 255));
  SelectObject(hdc, g_hFontLarge);

  // Use GetSystemPowerStatus for premium dynamic status text
  SYSTEM_POWER_STATUS sps;
  bool hasPowerStatus = GetSystemPowerStatus(&sps) != 0;
  bool isPluggedIn = hasPowerStatus && (sps.ACLineStatus == 1);
  bool isCharging = hasPowerStatus && (sps.BatteryFlag & 8); // 8 = Charging

  wchar_t timeText[64];
  wchar_t subtitleText[128];

  int actualPercent = (hasPowerStatus && sps.BatteryLifePercent != 255)
                          ? sps.BatteryLifePercent
                          : g_ecBatteryLevel;

  if (isPluggedIn) {
    if (isCharging) {
      swprintf_s(timeText, L_CHARGING[g_currentLanguage]);
      swprintf_s(subtitleText, L_LEVEL_TEXT[g_currentLanguage], actualPercent);
    } else {
      swprintf_s(timeText, L_PLUGGED_IN[g_currentLanguage]);
      if (g_ecBatteryCareEnabled) {
        swprintf_s(subtitleText, L_CARE_LIMIT_STATUS[g_currentLanguage],
                   g_ecBatteryLevel, actualPercent);
      } else {
        swprintf_s(subtitleText, L_BATTERY_CHARGED[g_currentLanguage],
                   actualPercent);
      }
    }
  } else {
    // Discharging: estimate remaining time based on current battery level and
    // performance mode
    float modeMultiplier = 1.0f;
    if (g_selectedPerformanceMode == 0)
      modeMultiplier = 1.2f; // Eco - longer battery life
    else if (g_selectedPerformanceMode == 2)
      modeMultiplier = 0.65f; // Turbo - shorter battery life

    int baseMinutes = (int)(actualPercent * 9 * 0.6f *
                            modeMultiplier); // Base: ~9h at 100% in Smart mode
    int hours = baseMinutes / 60;
    int minutes = baseMinutes % 60;

    swprintf_s(timeText, L_DISCHARGING_TIME[g_currentLanguage], hours, minutes);
    swprintf_s(subtitleText, L_DISCHARGING_SUB[g_currentLanguage],
               actualPercent);
  }

  RECT timeRc = {50, 130, 300, 170};
  DrawTextW(hdc, timeText, -1, &timeRc, DT_LEFT | DT_TOP | DT_SINGLELINE);

  // Battery percentage subtitle
  SelectObject(hdc, g_hFontNormal);
  RECT subRc = {50, 172, 340, 195};
  DrawTextW(hdc, subtitleText, -1, &subRc, DT_LEFT | DT_TOP | DT_SINGLELINE);

  SelectObject(hdc, hOldBrush2); // Restore old brush before deleting
  DeleteObject(hBatteryCardBrush);

  // ========== SETTINGS CARDS ==========
  SelectObject(hdc, g_hCardBrush);

  // Card 2: Charge Maintenance (shifted up and now the primary settings card at
  // y=255 to y=380 for better spacing)
  RoundRect(hdc, 32, 255, 760, 380, 12, 12);

  // Card 2 content: Charge Maintenance
  SetTextColor(hdc, RGB(255, 255, 255));
  SelectObject(hdc, g_hFontHeader);
  RECT card2Title = {50, 270, 400, 295};
  DrawTextW(hdc, L_CARE_TITLE[g_currentLanguage], -1, &card2Title,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);

  SetTextColor(hdc, RGB(170, 170, 170));
  SelectObject(hdc, g_hFontNormal);
  RECT card2Desc = {50, 298, 680, 328};
  DrawTextW(hdc, L_CARE_DESC[g_currentLanguage], -1, &card2Desc,
            DT_LEFT | DT_TOP | DT_WORDBREAK);

  // Restore objects
  SelectObject(hdc, hOldFont);
  SelectObject(hdc, hOldPen);
  SelectObject(hdc, hOldBrush);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
  switch (uMsg) {
  case WM_CREATE:
    g_uTaskbarRestart = RegisterWindowMessage(L"TaskbarCreated");
    g_isXiaomiHardware = IsXiaomiHardware();
    CreateControls(hwnd);
    if (InitializeWinRing0()) {
      UpdateBatteryCareStatus();
      UpdatePerformanceMode();
    }
    UpdateLanguageUI();
    if (!RegisterHotKey(hwnd, ID_HOTKEY_BATTERY, MOD_CONTROL | MOD_ALT, 'B')) {
      // If Ctrl+Alt+B failed (e.g. taken by another app), try Ctrl+Alt+L
      // (Limit) as fallback!
      RegisterHotKey(hwnd, ID_HOTKEY_BATTERY, MOD_CONTROL | MOD_ALT, 'L');
    }
    RegisterHotKey(hwnd, ID_HOTKEY_PERFORMANCE, MOD_CONTROL | MOD_ALT, 'P');
    SetTimer(hwnd, 1, 1000, nullptr);
    SetTimer(hwnd, ID_AUTO_REFRESH_TIMER, 2000, nullptr);
    break;

  case WM_DRAWITEM: {
    LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
    if (lpDrawItem->CtlType == ODT_BUTTON) {
      HDC realDC = lpDrawItem->hDC;
      RECT rc = lpDrawItem->rcItem;
      UINT id = lpDrawItem->CtlID;
      UINT state = lpDrawItem->itemState;
      HWND hwndItem = lpDrawItem->hwndItem;

      bool isHovered = GetPropW(hwndItem, L"Hover") != nullptr;
      bool isPressed = (state & ODS_SELECTED) != 0;

      // Double-buffering for flicker-free owner-drawn buttons
      HDC memDC = CreateCompatibleDC(realDC);
      HBITMAP memBM = CreateCompatibleBitmap(realDC, rc.right - rc.left,
                                             rc.bottom - rc.top);
      HBITMAP oldBM = (HBITMAP)SelectObject(memDC, memBM);
      HDC hDC = memDC;

      // Clear with parent background
      HBRUSH hParentBrush =
          (id == ID_CLOSE_BUTTON || id == ID_MIN_BUTTON || id == ID_GEAR_BUTTON)
              ? g_hBgBrush
              : g_hCardBrush;
      FillRect(hDC, &rc, hParentBrush);

      // Performance mode buttons
      if (id == ID_PERF_BTN_ECO || id == ID_PERF_BTN_SMART ||
          id == ID_PERF_BTN_TURBO) {
        bool isActive = false;
        if (id == ID_PERF_BTN_ECO && g_selectedPerformanceMode == 0)
          isActive = true;
        else if (id == ID_PERF_BTN_SMART && g_selectedPerformanceMode == 1)
          isActive = true;
        else if (id == ID_PERF_BTN_TURBO && g_selectedPerformanceMode == 2)
          isActive = true;

        // Clear with window background
        FillRect(hDC, &rc, g_hBgBrush);

        // Button background and pen (color-coded per mode as requested by user)
        HBRUSH hBtnBrush = nullptr;
        HPEN hBorderPenToUse = g_hBorderPen;

        if (isActive) {
          if (id == ID_PERF_BTN_ECO)
            hBtnBrush = g_hEcoBrush;
          else if (id == ID_PERF_BTN_SMART)
            hBtnBrush = g_hSmartBrush;
          else if (id == ID_PERF_BTN_TURBO)
            hBtnBrush = g_hTurboBrush;
        } else {
          if (isPressed)
            hBtnBrush = g_hButtonPressedBrush;
          else if (isHovered) {
            hBtnBrush = g_hButtonHoverBrush;
            if (id == ID_PERF_BTN_ECO)
              hBorderPenToUse = g_hEcoPen;
            else if (id == ID_PERF_BTN_SMART)
              hBorderPenToUse = g_hSmartPen;
            else if (id == ID_PERF_BTN_TURBO)
              hBorderPenToUse = g_hTurboPen;
          } else
            hBtnBrush = g_hCardBrush;
        }

        HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBtnBrush);
        HPEN hOldPen = (HPEN)SelectObject(
            hDC, isActive ? GetStockObject(NULL_PEN) : hBorderPenToUse);
        RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, 16, 16);
        SelectObject(hDC, hOldBrush);
        SelectObject(hDC, hOldPen);

        // Icon rendering with GDI+ antialiasing
        Gdiplus::Graphics graphics(hDC);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

        int cx = rc.left + (rc.right - rc.left) / 2;
        int cy = rc.top + 28;

        COLORREF iconColor =
            isActive ? RGB(255, 255, 255)
                     : (isHovered ? RGB(200, 200, 200) : RGB(140, 140, 140));
        Gdiplus::Color gdipColor(255, GetRValue(iconColor),
                                 GetGValue(iconColor), GetBValue(iconColor));
        Gdiplus::Pen iconPen(gdipColor, 2.0f);
        iconPen.SetLineCap(Gdiplus::LineCapRound, Gdiplus::LineCapRound,
                           Gdiplus::DashCapRound);
        Gdiplus::SolidBrush iconBrush(gdipColor);

        if (id == ID_PERF_BTN_ECO) {
          // Beautiful leaf icon with smooth curves
          Gdiplus::GraphicsPath leafPath;

          // Leaf body using bezier curves for smooth shape
          leafPath.AddBezier((float)cx - 4,
                             (float)cy + 6, // start (stem connection)
                             (float)cx - 10, (float)cy - 2, // control 1
                             (float)cx - 4, (float)cy - 10, // control 2
                             (float)cx + 8, (float)cy - 8); // end (tip)
          leafPath.AddBezier((float)cx + 8, (float)cy - 8,  // start (tip)
                             (float)cx + 6, (float)cy + 2,  // control 1
                             (float)cx + 2, (float)cy + 6,  // control 2
                             (float)cx - 4,
                             (float)cy + 6); // end (stem connection)

          graphics.DrawPath(&iconPen, &leafPath);

          // Central vein
          graphics.DrawLine(&iconPen, (float)cx - 3, (float)cy + 4,
                            (float)cx + 5, (float)cy - 5);

          // Stem
          graphics.DrawLine(&iconPen, (float)cx - 4, (float)cy + 6,
                            (float)cx - 8, (float)cy + 10);

        } else if (id == ID_PERF_BTN_SMART) {
          // Clean CPU/chip icon
          Gdiplus::RectF cpuRect((float)cx - 7, (float)cy - 7, 14.0f, 14.0f);
          graphics.DrawRectangle(&iconPen, cpuRect);

          // Inner core
          Gdiplus::RectF coreRect((float)cx - 3, (float)cy - 3, 6.0f, 6.0f);
          graphics.FillRectangle(&iconBrush, coreRect);

          // Pins with rounded ends
          float pinLen = 4.0f;
          // Top pins
          graphics.DrawLine(&iconPen, (float)cx - 4, (float)cy - 7,
                            (float)cx - 4, (float)cy - 7 - pinLen);
          graphics.DrawLine(&iconPen, (float)cx, (float)cy - 7, (float)cx,
                            (float)cy - 7 - pinLen);
          graphics.DrawLine(&iconPen, (float)cx + 4, (float)cy - 7,
                            (float)cx + 4, (float)cy - 7 - pinLen);
          // Bottom pins
          graphics.DrawLine(&iconPen, (float)cx - 4, (float)cy + 7,
                            (float)cx - 4, (float)cy + 7 + pinLen);
          graphics.DrawLine(&iconPen, (float)cx, (float)cy + 7, (float)cx,
                            (float)cy + 7 + pinLen);
          graphics.DrawLine(&iconPen, (float)cx + 4, (float)cy + 7,
                            (float)cx + 4, (float)cy + 7 + pinLen);
          // Left pins
          graphics.DrawLine(&iconPen, (float)cx - 7, (float)cy - 4,
                            (float)cx - 7 - pinLen, (float)cy - 4);
          graphics.DrawLine(&iconPen, (float)cx - 7, (float)cy,
                            (float)cx - 7 - pinLen, (float)cy);
          graphics.DrawLine(&iconPen, (float)cx - 7, (float)cy + 4,
                            (float)cx - 7 - pinLen, (float)cy + 4);
          // Right pins
          graphics.DrawLine(&iconPen, (float)cx + 7, (float)cy - 4,
                            (float)cx + 7 + pinLen, (float)cy - 4);
          graphics.DrawLine(&iconPen, (float)cx + 7, (float)cy,
                            (float)cx + 7 + pinLen, (float)cy);
          graphics.DrawLine(&iconPen, (float)cx + 7, (float)cy + 4,
                            (float)cx + 7 + pinLen, (float)cy + 4);

        } else if (id == ID_PERF_BTN_TURBO) {
          // Lightning bolt with filled shape
          Gdiplus::GraphicsPath boltPath;
          Gdiplus::PointF boltPts[] = {
              {(float)cx + 2, (float)cy - 10}, // Top
              {(float)cx - 6, (float)cy + 1},  // Middle left
              {(float)cx - 1, (float)cy + 1},  // Inner left
              {(float)cx - 2, (float)cy + 10}, // Bottom
              {(float)cx + 6, (float)cy - 1},  // Middle right
              {(float)cx + 1, (float)cy - 1}   // Inner right
          };
          boltPath.AddPolygon(boltPts, 6);

          if (isActive) {
            graphics.FillPath(&iconBrush, &boltPath);
          } else {
            graphics.DrawPath(&iconPen, &boltPath);
          }
        }

        // Button text
        wchar_t text[64];
        GetWindowTextW(hwndItem, text, 64);
        SetBkMode(hDC, TRANSPARENT);
        SetTextColor(hDC, isActive ? RGB(255, 255, 255)
                                   : (isHovered ? RGB(220, 220, 220)
                                                : RGB(180, 180, 180)));

        RECT textRc = {rc.left, rc.top + 50, rc.right, rc.bottom - 8};
        HFONT hOldFont = (HFONT)SelectObject(hDC, g_hFontNormal);
        DrawTextW(hDC, text, -1, &textRc, DT_CENTER | DT_TOP | DT_SINGLELINE);
        SelectObject(hDC, hOldFont);

        goto cleanup_button;
      }

      // Toggle switches
      if (id == ID_BATTERY_CARE_TOGGLE) {
        bool isOn = g_ecBatteryCareEnabled;
        DrawToggle(hDC, rc.left, rc.top, isOn, isHovered);
        goto cleanup_button;
      }

      // Gear button drawing with GDI+ antialiasing
      if (id == ID_GEAR_BUTTON) {
        HBRUSH hBgBrush = nullptr;
        if (isPressed)
          hBgBrush = g_hButtonPressedBrush;
        else if (isHovered)
          hBgBrush = g_hHoverBrush;
        else
          hBgBrush = g_hBgBrush;

        HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBgBrush);
        HPEN hOldPen = (HPEN)SelectObject(hDC, GetStockObject(NULL_PEN));
        RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, 6, 6);
        SelectObject(hDC, hOldBrush);
        SelectObject(hDC, hOldPen);

        // Draw gear icon with GDI+ for smooth antialiasing
        Gdiplus::Graphics graphics(hDC);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

        int cx = rc.left + (rc.right - rc.left) / 2;
        int cy = rc.top + (rc.bottom - rc.top) / 2;

        COLORREF iconColor =
            isHovered ? RGB(255, 255, 255) : RGB(200, 200, 200);
        Gdiplus::Pen gearPen(Gdiplus::Color(255, GetRValue(iconColor),
                                            GetGValue(iconColor),
                                            GetBValue(iconColor)),
                             1.5f);

        // Draw outer gear shape with teeth
        Gdiplus::GraphicsPath gearPath;
        float outerR = 7.0f;
        float innerR = 4.5f;
        float teethHeight = 2.5f;
        int teethCount = 8;

        for (int i = 0; i < teethCount; i++) {
          float angle1 = (float)(i * 360.0 / teethCount) * 3.14159f / 180.0f;
          float angle2 =
              (float)((i + 0.35) * 360.0 / teethCount) * 3.14159f / 180.0f;
          float angle3 =
              (float)((i + 0.65) * 360.0 / teethCount) * 3.14159f / 180.0f;
          float angle4 =
              (float)((i + 1) * 360.0 / teethCount) * 3.14159f / 180.0f;

          float r1 = outerR + teethHeight;
          float r2 = outerR;

          if (i == 0) {
            gearPath.StartFigure();
          }
          gearPath.AddLine(cx + r1 * cosf(angle1), cy + r1 * sinf(angle1),
                           cx + r1 * cosf(angle2), cy + r1 * sinf(angle2));
          gearPath.AddLine(cx + r1 * cosf(angle2), cy + r1 * sinf(angle2),
                           cx + r2 * cosf(angle3), cy + r2 * sinf(angle3));
          gearPath.AddLine(cx + r2 * cosf(angle3), cy + r2 * sinf(angle3),
                           cx + r2 * cosf(angle4), cy + r2 * sinf(angle4));
        }
        gearPath.CloseFigure();

        graphics.DrawPath(&gearPen, &gearPath);

        // Inner circle (center hole)
        graphics.DrawEllipse(&gearPen, (float)cx - 3.0f, (float)cy - 3.0f, 6.0f,
                             6.0f);

        goto cleanup_button;
      }

      // Settings back button drawing
      if (id == ID_SETTINGS_BACK_BTN) {
        HBRUSH hBgBrush = nullptr;
        if (isPressed)
          hBgBrush = g_hButtonPressedBrush;
        else if (isHovered)
          hBgBrush = g_hButtonHoverBrush;
        else
          hBgBrush = g_hButtonBrush;

        HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBgBrush);
        HPEN hOldPen =
            (HPEN)SelectObject(hDC, isHovered ? g_hAccentPen : g_hBorderPen);
        RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, 6, 6);
        SelectObject(hDC, hOldBrush);
        SelectObject(hDC, hOldPen);

        wchar_t text[64];
        GetWindowTextW(hwndItem, text, 64);
        SetBkMode(hDC, TRANSPARENT);
        SetTextColor(hDC, RGB(240, 240, 240));
        HFONT hOldFont = (HFONT)SelectObject(hDC, g_hFontNormal);
        DrawTextW(hDC, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hDC, hOldFont);

        goto cleanup_button;
      }

      // Preset buttons (50%, 60%, 70%, 80%, 90%, 100%)
      if (id == ID_PRESET_50 || id == ID_PRESET_60 || id == ID_PRESET_70 ||
          id == ID_PRESET_80 || id == ID_PRESET_90 || id == ID_PRESET_100) {
        int presetVal = 80;
        if (id == ID_PRESET_50)
          presetVal = 50;
        else if (id == ID_PRESET_60)
          presetVal = 60;
        else if (id == ID_PRESET_70)
          presetVal = 70;
        else if (id == ID_PRESET_80)
          presetVal = 80;
        else if (id == ID_PRESET_90)
          presetVal = 90;
        else if (id == ID_PRESET_100)
          presetVal = 100;

        bool isActive =
            (g_ecBatteryLevel == presetVal) && g_ecBatteryCareEnabled;
        bool isEnabled = g_ecBatteryCareEnabled;

        // Clear area with card background
        FillRect(hDC, &rc, g_hCardBrush);

        HBRUSH hBtnBrush = nullptr;
        if (!isEnabled) {
          hBtnBrush = g_hBgBrush; // Disabled state: matches window background
        } else if (isActive) {
          hBtnBrush = g_hAccentBrush; // Green when active
        } else {
          if (isPressed)
            hBtnBrush = g_hButtonPressedBrush;
          else if (isHovered)
            hBtnBrush = g_hButtonHoverBrush;
          else
            hBtnBrush = g_hBgBrush; // Inactive state: matches window background
        }

        HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBtnBrush);
        HPEN hOldPen = (HPEN)SelectObject(
            hDC, isActive ? GetStockObject(NULL_PEN)
                          : (!isEnabled
                                 ? g_hBorderPen
                                 : (isHovered ? g_hAccentPen : g_hBorderPen)));

        RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, 15, 15);
        SelectObject(hDC, hOldBrush);
        SelectObject(hDC, hOldPen);

        // Draw text
        wchar_t text[16];
        GetWindowTextW(hwndItem, text, 16);
        SetBkMode(hDC, TRANSPARENT);
        if (!isEnabled) {
          SetTextColor(hDC, RGB(100, 100, 100)); // Gray text when disabled
        } else {
          SetTextColor(hDC, isActive ? RGB(255, 255, 255)
                                     : (isHovered ? RGB(220, 220, 220)
                                                  : RGB(180, 180, 180)));
        }

        HFONT hOldFont = (HFONT)SelectObject(hDC, g_hFontNormal);
        DrawTextW(hDC, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hDC, hOldFont);

        goto cleanup_button;
      }

      // Window control buttons
      if (id == ID_CLOSE_BUTTON || id == ID_MIN_BUTTON) {
        HBRUSH hBgBrush = nullptr;
        if (id == ID_CLOSE_BUTTON) {
          if (isPressed)
            hBgBrush = g_hClosePressedBrush;
          else if (isHovered)
            hBgBrush = g_hCloseHoverBrush;
          else
            hBgBrush = g_hBgBrush;
        } else {
          if (isPressed)
            hBgBrush = g_hButtonPressedBrush;
          else if (isHovered)
            hBgBrush = g_hHoverBrush;
          else
            hBgBrush = g_hBgBrush;
        }

        HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBgBrush);
        HPEN hOldPen = (HPEN)SelectObject(hDC, GetStockObject(NULL_PEN));
        RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, 6, 6);
        SelectObject(hDC, hOldBrush);
        SelectObject(hDC, hOldPen);

        wchar_t text[8];
        GetWindowTextW(hwndItem, text, 8);
        SetBkMode(hDC, TRANSPARENT);
        SetTextColor(hDC, RGB(220, 220, 220));
        SelectObject(hDC, g_hFontHeader);
        DrawTextW(hDC, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        goto cleanup_button;
      }

    cleanup_button:
      BitBlt(realDC, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
             memDC, 0, 0, SRCCOPY);
      SelectObject(memDC, oldBM);
      DeleteObject(memBM);
      DeleteDC(memDC);
      return TRUE;
    }
    break;
  }

  case WM_TIMER:
    if (wParam == 1) {
      KillTimer(hwnd, 1);
      CreateTrayIcon();
    } else if (wParam == ID_TRAY_RETRY_TIMER) {
      CreateTrayIcon();
    } else if (wParam == ID_AUTO_REFRESH_TIMER) {
      UpdateBatteryCareStatus();
      UpdatePerformanceMode();
    }
    break;

  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rect;
    GetClientRect(hwnd, &rect);

    // Double buffering
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBM = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    HBITMAP oldBM = (HBITMAP)SelectObject(memDC, memBM);

    DrawBackground(memDC, &rect);

    BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, oldBM);
    DeleteObject(memBM);
    DeleteDC(memDC);

    EndPaint(hwnd, &ps);
    break;
  }

  case WM_ERASEBKGND:
    return 1;

  case WM_LBUTTONDOWN:
    g_isDragging = true;
    g_dragOffset.x = LOWORD(lParam);
    g_dragOffset.y = HIWORD(lParam);
    SetCapture(hwnd);
    break;

  case WM_LBUTTONUP:
    if (g_isDragging) {
      g_isDragging = false;
      ReleaseCapture();
    }
    break;

  case WM_MOUSEMOVE:
    if (g_isDragging) {
      POINT pt;
      GetCursorPos(&pt);
      SetWindowPos(hwnd, nullptr, pt.x - g_dragOffset.x, pt.y - g_dragOffset.y,
                   0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }
    break;

  case WM_CTLCOLORSTATIC: {
    HDC hdcStatic = (HDC)wParam;
    SetBkMode(hdcStatic, TRANSPARENT);

    HWND hStatic = (HWND)lParam;
    if (hStatic == g_hTitleLabel) {
      SetTextColor(hdcStatic, RGB(255, 255, 255));
      return (LRESULT)g_hBgBrush;
    }
    if (hStatic == g_hPerfDescLabel) {
      SetTextColor(hdcStatic, RGB(170, 170, 170));
      return (LRESULT)g_hBgBrush;
    }
    if (hStatic == g_hAutostartText || hStatic == g_hMinimizeText) {
      SetTextColor(hdcStatic, RGB(220, 220, 220));
      return (
          LRESULT)g_hCardBrush; // Paint checkbox labels with card background
    }

    SetTextColor(hdcStatic, RGB(200, 200, 200));
    return (LRESULT)g_hCardBrush;
  }

  // Handle checkbox button colors (for BS_AUTOCHECKBOX)
  case WM_CTLCOLORBTN: {
    HDC hdcBtn = (HDC)wParam;
    HWND hBtn = (HWND)lParam;
    SetBkMode(hdcBtn, TRANSPARENT);

    if (hBtn == g_hAutostartCheck || hBtn == g_hMinimizeCheck) {
      SetTextColor(hdcBtn, RGB(220, 220, 220));
      return (LRESULT)g_hCardBrush;
    }
    return (LRESULT)g_hBgBrush;
  }

  case WM_COMMAND:
    // Handle Combobox Language Selection change
    if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == ID_LANG_COMBO) {
      int sel = (int)SendMessage(g_hLangCombo, CB_GETCURSEL, 0, 0);
      if (sel != CB_ERR) {
        g_currentLanguage = (Language)sel;
        UpdateLanguageUI();
        SaveConfig();
      }
    }

    switch (LOWORD(wParam)) {
    case ID_GEAR_BUTTON:
      g_showSettings = true;
      ToggleSettingsUI(true);
      InvalidateRect(hwnd, nullptr, FALSE);
      break;

    case ID_SETTINGS_BACK_BTN:
      g_showSettings = false;
      ToggleSettingsUI(false);
      InvalidateRect(hwnd, nullptr, FALSE);
      break;

    case ID_AUTOSTART_CHECK: {
      LRESULT chk = SendMessage(g_hAutostartCheck, BM_GETCHECK, 0, 0);
      SetAutostart(chk == BST_CHECKED);
      SaveConfig();
      break;
    }

    case ID_AUTOSTART_TEXT: {
      LRESULT chk = SendMessage(g_hAutostartCheck, BM_GETCHECK, 0, 0);
      LRESULT newChk = (chk == BST_CHECKED) ? BST_UNCHECKED : BST_CHECKED;
      SendMessage(g_hAutostartCheck, BM_SETCHECK, newChk, 0);
      SetAutostart(newChk == BST_CHECKED);
      SaveConfig();
      break;
    }

    case ID_MINIMIZE_CHECK: {
      LRESULT chk = SendMessage(g_hMinimizeCheck, BM_GETCHECK, 0, 0);
      g_minimizeToTrayOnClose = (chk == BST_CHECKED);
      SaveConfig();
      break;
    }

    case ID_MINIMIZE_TEXT: {
      LRESULT chk = SendMessage(g_hMinimizeCheck, BM_GETCHECK, 0, 0);
      LRESULT newChk = (chk == BST_CHECKED) ? BST_UNCHECKED : BST_CHECKED;
      SendMessage(g_hMinimizeCheck, BM_SETCHECK, newChk, 0);
      g_minimizeToTrayOnClose = (newChk == BST_CHECKED);
      SaveConfig();
      break;
    }

    case ID_PERF_BTN_ECO:
      g_selectedPerformanceMode = 0;
      SetPerformanceMode(0);
      UpdatePerformanceMode();
      ShowOSD(0);
      SaveConfig();
      break;

    case ID_PERF_BTN_SMART:
      g_selectedPerformanceMode = 1;
      SetPerformanceMode(1);
      UpdatePerformanceMode();
      ShowOSD(1);
      SaveConfig();
      break;

    case ID_PERF_BTN_TURBO:
      g_selectedPerformanceMode = 2;
      SetPerformanceMode(2);
      UpdatePerformanceMode();
      ShowOSD(2);
      SaveConfig();
      break;

    case ID_BATTERY_CARE_TOGGLE:
      g_ecBatteryCareEnabled = !g_ecBatteryCareEnabled;
      SetBatteryCare(g_ecBatteryCareEnabled, g_ecBatteryLevel);
      UpdateBatteryCareStatus();
      ShowOSD(g_ecBatteryCareEnabled ? 3 : 4);
      SaveConfig();
      InvalidateRect(hwnd, nullptr, FALSE);
      break;

    case ID_PRESET_50:
    case ID_PRESET_60:
    case ID_PRESET_70:
    case ID_PRESET_80:
    case ID_PRESET_90:
    case ID_PRESET_100:
      if (g_ecBatteryCareEnabled) {
        int presetVal = 80;
        if (LOWORD(wParam) == ID_PRESET_50)
          presetVal = 50;
        else if (LOWORD(wParam) == ID_PRESET_60)
          presetVal = 60;
        else if (LOWORD(wParam) == ID_PRESET_70)
          presetVal = 70;
        else if (LOWORD(wParam) == ID_PRESET_80)
          presetVal = 80;
        else if (LOWORD(wParam) == ID_PRESET_90)
          presetVal = 90;
        else if (LOWORD(wParam) == ID_PRESET_100)
          presetVal = 100;

        g_ecBatteryLevel = presetVal;
        SetBatteryCare(g_ecBatteryCareEnabled, g_ecBatteryLevel);
        UpdateBatteryCareStatus();
        SaveConfig();
        InvalidateRect(hwnd, nullptr, FALSE);
      }
      break;

    case ID_MIN_BUTTON:
      ShowWindow(hwnd, SW_HIDE);
      g_isMinimized = true;
      break;

    case ID_CLOSE_BUTTON:
      if (g_minimizeToTrayOnClose) {
        ShowWindow(hwnd, SW_HIDE);
        g_isMinimized = true;
      } else {
        DestroyWindow(hwnd);
      }
      break;

    case ID_TRAY_SHOW:
      ToggleMainWindow();
      break;

    case ID_TRAY_EXIT:
      DestroyWindow(hwnd);
      break;

    case ID_TRAY_BATTERY_ENABLE:
      SetBatteryCare(true, g_ecBatteryLevel);
      g_ecBatteryCareEnabled = true;
      UpdateBatteryCareStatus();
      ShowOSD(3);
      SaveConfig();
      break;

    case ID_TRAY_BATTERY_DISABLE:
      SetBatteryCare(false, g_ecBatteryLevel);
      g_ecBatteryCareEnabled = false;
      UpdateBatteryCareStatus();
      ShowOSD(4);
      SaveConfig();
      break;

    case ID_TRAY_PERF_ECO:
      SetPerformanceMode(0);
      g_selectedPerformanceMode = 0;
      UpdatePerformanceMode();
      ShowOSD(0);
      SaveConfig();
      break;

    case ID_TRAY_PERF_SMART:
      SetPerformanceMode(1);
      g_selectedPerformanceMode = 1;
      UpdatePerformanceMode();
      ShowOSD(1);
      SaveConfig();
      break;

    case ID_TRAY_PERF_FAST:
      SetPerformanceMode(2);
      g_selectedPerformanceMode = 2;
      UpdatePerformanceMode();
      ShowOSD(2);
      SaveConfig();
      break;
    }
    break;

  case WM_TRAYICON:
    switch (lParam) {
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONUP:
      ToggleMainWindow();
      break;
    case WM_RBUTTONUP:
    case WM_CONTEXTMENU:
      ShowTrayMenu();
      break;
    }
    break;

  case WM_HOTKEY:
    if (wParam == ID_HOTKEY_BATTERY) {
      g_ecBatteryCareEnabled = !g_ecBatteryCareEnabled;
      SetBatteryCare(g_ecBatteryCareEnabled, g_ecBatteryLevel);
      UpdateBatteryCareStatus();
      ShowOSD(g_ecBatteryCareEnabled ? 3 : 4);
      SaveConfig();
    } else if (wParam == ID_HOTKEY_PERFORMANCE) {
      g_selectedPerformanceMode = (g_selectedPerformanceMode + 1) % 3;
      SetPerformanceMode(g_selectedPerformanceMode);
      UpdatePerformanceMode();
      ShowOSD(g_selectedPerformanceMode);
      SaveConfig();
    }
    break;

  case WM_ENDSESSION:
    if (wParam == TRUE)
      SaveConfig();
    break;

  case WM_CLOSE:
    if (g_minimizeToTrayOnClose) {
      ShowWindow(hwnd, SW_HIDE);
      g_isMinimized = true;
    } else {
      DestroyWindow(hwnd);
    }
    break;

  case WM_DESTROY:
    if (g_hwndOSD) {
      DestroyWindow(g_hwndOSD);
      g_hwndOSD = nullptr;
    }
    SaveConfig();
    RemoveTrayIcon();
    DeinitializeWinRing0();
    UnregisterHotKey(hwnd, ID_HOTKEY_BATTERY);
    UnregisterHotKey(hwnd, ID_HOTKEY_PERFORMANCE);

    if (g_hFontNormal)
      DeleteObject(g_hFontNormal);
    if (g_hFontTitle)
      DeleteObject(g_hFontTitle);
    if (g_hFontHeader)
      DeleteObject(g_hFontHeader);
    if (g_hFontSmall)
      DeleteObject(g_hFontSmall);
    if (g_hFontLarge)
      DeleteObject(g_hFontLarge);
    if (g_hFontMedium)
      DeleteObject(g_hFontMedium);

    if (g_hBgBrush)
      DeleteObject(g_hBgBrush);
    if (g_hCardBrush)
      DeleteObject(g_hCardBrush);
    if (g_hAccentBrush)
      DeleteObject(g_hAccentBrush);
    if (g_hAccentHoverBrush)
      DeleteObject(g_hAccentHoverBrush);
    if (g_hHoverBrush)
      DeleteObject(g_hHoverBrush);
    if (g_hButtonBrush)
      DeleteObject(g_hButtonBrush);
    if (g_hButtonHoverBrush)
      DeleteObject(g_hButtonHoverBrush);
    if (g_hButtonPressedBrush)
      DeleteObject(g_hButtonPressedBrush);
    if (g_hCloseHoverBrush)
      DeleteObject(g_hCloseHoverBrush);
    if (g_hClosePressedBrush)
      DeleteObject(g_hClosePressedBrush);
    if (g_hToggleOffBrush)
      DeleteObject(g_hToggleOffBrush);
    if (g_hToggleOnBrush)
      DeleteObject(g_hToggleOnBrush);
    if (g_hBorderPen)
      DeleteObject(g_hBorderPen);
    if (g_hAccentPen)
      DeleteObject(g_hAccentPen);

    // Delete color-coded brushes and pens
    if (g_hEcoBrush)
      DeleteObject(g_hEcoBrush);
    if (g_hSmartBrush)
      DeleteObject(g_hSmartBrush);
    if (g_hTurboBrush)
      DeleteObject(g_hTurboBrush);
    if (g_hEcoPen)
      DeleteObject(g_hEcoPen);
    if (g_hSmartPen)
      DeleteObject(g_hSmartPen);
    if (g_hTurboPen)
      DeleteObject(g_hTurboPen);

    PostQuitMessage(0);
    break;

  case WM_SYSCOMMAND:
    if (wParam == SC_MINIMIZE) {
      ShowWindow(hwnd, SW_HIDE);
      g_isMinimized = true;
      return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);

  default:
    if (uMsg == g_uTaskbarRestart)
      CreateTrayIcon();
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
  return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPWSTR lpCmdLine, int nCmdShow) {
  g_hInstance = hInstance;

  CoInitializeEx(NULL, COINIT_MULTITHREADED);

  // Initialize GDI+ for smooth antialiased icon rendering
  Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;
  Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

  wchar_t exePath[MAX_PATH];
  GetModuleFileNameW(nullptr, exePath, MAX_PATH);
  wchar_t *lastSlash = wcsrchr(exePath, L'\\');
  if (lastSlash) {
    *lastSlash = L'\0';
    SetCurrentDirectoryW(exePath);
  }

  LoadConfig();

  INITCOMMONCONTROLSEX icex;
  icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
  icex.dwICC = ICC_STANDARD_CLASSES | ICC_BAR_CLASSES;
  InitCommonControlsEx(&icex);

  WNDCLASSEXW wc = {};
  wc.cbSize = sizeof(WNDCLASSEXW);
  wc.style = 0;
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;
  wc.hbrBackground = nullptr;
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON), IMAGE_ICON,
                             GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON),
                             LR_DEFAULTCOLOR | LR_SHARED);
  wc.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON), IMAGE_ICON,
                               GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
                               LR_DEFAULTCOLOR | LR_SHARED);
  if (!wc.hIcon)
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);

  if (!RegisterClassExW(&wc)) {
    MessageBox(nullptr, L_MSG_REG_CLASS_FAIL[g_currentLanguage],
               L_MSG_ERROR[g_currentLanguage], MB_OK | MB_ICONERROR);
    return 0;
  }

  // Initialize fonts (Segoe UI for Windows 10/11 native look)
  g_hFontNormal =
      CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
  g_hFontTitle =
      CreateFontW(16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                  CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
  g_hFontHeader =
      CreateFontW(15, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                  CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
  g_hFontSmall =
      CreateFontW(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
  g_hFontLarge =
      CreateFontW(32, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
  g_hFontMedium =
      CreateFontW(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

  // PC Manager Dark Theme colors
  g_hBgBrush = CreateSolidBrush(RGB(26, 26, 26));            // #1a1a1a
  g_hCardBrush = CreateSolidBrush(RGB(37, 37, 37));          // #252525
  g_hAccentBrush = CreateSolidBrush(RGB(34, 197, 94));       // #22c55e (Green)
  g_hAccentHoverBrush = CreateSolidBrush(RGB(22, 163, 74));  // #16a34a
  g_hHoverBrush = CreateSolidBrush(RGB(51, 51, 51));         // #333333
  g_hButtonBrush = CreateSolidBrush(RGB(45, 45, 45));        // #2d2d2d
  g_hButtonHoverBrush = CreateSolidBrush(RGB(64, 64, 64));   // #404040
  g_hButtonPressedBrush = CreateSolidBrush(RGB(26, 26, 26)); // #1a1a1a
  g_hCloseHoverBrush = CreateSolidBrush(RGB(239, 68, 68));   // #ef4444
  g_hClosePressedBrush = CreateSolidBrush(RGB(220, 38, 38)); // #dc2626
  g_hToggleOffBrush = CreateSolidBrush(RGB(64, 64, 64));     // #404040
  g_hToggleOnBrush = CreateSolidBrush(RGB(34, 197, 94));     // #22c55e

  g_hBorderPen = CreatePen(PS_SOLID, 1, RGB(51, 51, 51));  // #333333
  g_hAccentPen = CreatePen(PS_SOLID, 1, RGB(34, 197, 94)); // #22c55e

  // Initialize color-coded brushes and pens for performance modes
  g_hEcoBrush = CreateSolidBrush(RGB(34, 197, 94));    // Green (#22c55e)
  g_hSmartBrush = CreateSolidBrush(RGB(59, 130, 246)); // Blue (#3b82f6)
  g_hTurboBrush =
      CreateSolidBrush(RGB(234, 88, 12)); // Vibrant Orange (#ea580c)

  g_hEcoPen = CreatePen(PS_SOLID, 1, RGB(34, 197, 94));
  g_hSmartPen = CreatePen(PS_SOLID, 1, RGB(59, 130, 246));
  g_hTurboPen = CreatePen(PS_SOLID, 1, RGB(234, 88, 12));

  const int windowWidth = 810;
  const int windowHeight = 440;
  g_hWnd = CreateWindowExW(
      0, CLASS_NAME, WINDOW_TITLE, WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN,
      (GetSystemMetrics(SM_CXSCREEN) - windowWidth) / 2,
      (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2, windowWidth,
      windowHeight, nullptr, nullptr, hInstance, nullptr);

  if (g_hWnd == nullptr) {
    MessageBoxW(nullptr, L_MSG_CREATE_WINDOW_FAIL[g_currentLanguage],
                L_MSG_ERROR[g_currentLanguage], MB_OK | MB_ICONERROR);
    return 0;
  }

  ShowWindow(g_hWnd, nCmdShow);
  UpdateWindow(g_hWnd);

  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  // Shutdown GDI+
  Gdiplus::GdiplusShutdown(gdiplusToken);

  CoUninitialize();

  return (int)msg.wParam;
}

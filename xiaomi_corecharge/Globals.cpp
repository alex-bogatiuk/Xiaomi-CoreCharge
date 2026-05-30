#include "Globals.h"

const wchar_t *CLASS_NAME = L"XiaomiCoreCharge";
const wchar_t *WINDOW_TITLE = L"Xiaomi CoreCharge";
const wchar_t *CONFIG_FILE = L"xiaomi_corecharge_config.ini";

HWND g_hWnd = nullptr;
HWND g_hBatteryCareToggle = nullptr;
HWND g_hPerfBtnEco = nullptr;
HWND g_hPerfBtnSmart = nullptr;
HWND g_hPerfBtnTurbo = nullptr;
HWND g_hPerfDescLabel = nullptr;
HWND g_hBatteryStatusLabel = nullptr;
HWND g_hGearButton = nullptr;
HWND g_hLangCombo = nullptr;
HWND g_hAutostartCheck = nullptr;
HWND g_hMinimizeCheck = nullptr;
HWND g_hSettingsBackBtn = nullptr;
HWND g_hAutostartText = nullptr;
HWND g_hMinimizeText = nullptr;
HWND g_hPreset50 = nullptr;
HWND g_hPreset60 = nullptr;
HWND g_hPreset70 = nullptr;
HWND g_hPreset80 = nullptr;
HWND g_hPreset90 = nullptr;
HWND g_hPreset100 = nullptr;
HWND g_hTitleLabel = nullptr;
HWND g_hwndOSD = nullptr;

bool g_isXiaomiHardware = false;
bool g_minimizeToTrayOnClose = true;
bool g_showSettings = false;
Language g_currentLanguage = LANG_RU;

int g_selectedPerformanceMode = 1;
bool g_ecBatteryCareEnabled = false;
int g_ecBatteryLevel = 57;
int g_ecPerformanceMode = 1;
int g_osdMode = 0;
int g_osdAlpha = 225;
bool g_isMinimized = false;
bool g_isDragging = false;
POINT g_dragOffset = {};
HINSTANCE g_hInstance = nullptr;
bool g_configLoaded = false;

HFONT g_hFontNormal = nullptr;
HFONT g_hFontTitle = nullptr;
HFONT g_hFontHeader = nullptr;
HFONT g_hFontSmall = nullptr;
HFONT g_hFontLarge = nullptr;
HFONT g_hFontMedium = nullptr;

HBRUSH g_hBgBrush = nullptr;
HBRUSH g_hCardBrush = nullptr;
HBRUSH g_hAccentBrush = nullptr;
HBRUSH g_hAccentHoverBrush = nullptr;
HBRUSH g_hHoverBrush = nullptr;
HBRUSH g_hButtonBrush = nullptr;
HBRUSH g_hButtonHoverBrush = nullptr;
HBRUSH g_hButtonPressedBrush = nullptr;
HBRUSH g_hCloseHoverBrush = nullptr;
HBRUSH g_hClosePressedBrush = nullptr;
HBRUSH g_hToggleOffBrush = nullptr;
HBRUSH g_hToggleOnBrush = nullptr;
HPEN g_hBorderPen = nullptr;
HPEN g_hAccentPen = nullptr;

HBRUSH g_hEcoBrush = nullptr;
HBRUSH g_hSmartBrush = nullptr;
HBRUSH g_hTurboBrush = nullptr;
HPEN g_hEcoPen = nullptr;
HPEN g_hSmartPen = nullptr;
HPEN g_hTurboPen = nullptr;

UINT g_uTaskbarRestart = 0;
NOTIFYICONDATA g_nid = {};

HBITMAP g_hOsdBitmap = nullptr;
HDC g_osdMemDC = nullptr;
HBITMAP g_osdOldBitmap = nullptr;

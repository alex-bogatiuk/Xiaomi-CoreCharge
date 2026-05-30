#pragma once
#include <windows.h>
#include "Constants.h"

extern const wchar_t *CLASS_NAME;
extern const wchar_t *WINDOW_TITLE;
extern const wchar_t *CONFIG_FILE;

extern HWND g_hWnd;
extern HWND g_hBatteryCareToggle;
extern HWND g_hPerfBtnEco;
extern HWND g_hPerfBtnSmart;
extern HWND g_hPerfBtnTurbo;
extern HWND g_hPerfDescLabel;
extern HWND g_hBatteryStatusLabel;
extern HWND g_hGearButton;
extern HWND g_hLangCombo;
extern HWND g_hAutostartCheck;
extern HWND g_hMinimizeCheck;
extern HWND g_hSettingsBackBtn;
extern HWND g_hAutostartText;
extern HWND g_hMinimizeText;
extern HWND g_hPreset50;
extern HWND g_hPreset60;
extern HWND g_hPreset70;
extern HWND g_hPreset80;
extern HWND g_hPreset90;
extern HWND g_hPreset100;
extern HWND g_hTitleLabel;
extern HWND g_hwndOSD;

extern bool g_isXiaomiHardware;
extern bool g_minimizeToTrayOnClose;
extern bool g_showSettings;
extern Language g_currentLanguage;

extern int g_selectedPerformanceMode;
extern bool g_ecBatteryCareEnabled;
extern int g_ecBatteryLevel;
extern int g_ecPerformanceMode;
extern int g_osdMode;
extern int g_osdAlpha;
extern bool g_isMinimized;
extern bool g_isDragging;
extern POINT g_dragOffset;
extern HINSTANCE g_hInstance;
extern bool g_configLoaded;

// Fonts
extern HFONT g_hFontNormal;
extern HFONT g_hFontTitle;
extern HFONT g_hFontHeader;
extern HFONT g_hFontSmall;
extern HFONT g_hFontLarge;
extern HFONT g_hFontMedium;

// PC Manager Dark Theme Colors
extern HBRUSH g_hBgBrush;
extern HBRUSH g_hCardBrush;
extern HBRUSH g_hAccentBrush;
extern HBRUSH g_hAccentHoverBrush;
extern HBRUSH g_hHoverBrush;
extern HBRUSH g_hButtonBrush;
extern HBRUSH g_hButtonHoverBrush;
extern HBRUSH g_hButtonPressedBrush;
extern HBRUSH g_hCloseHoverBrush;
extern HBRUSH g_hClosePressedBrush;
extern HBRUSH g_hToggleOffBrush;
extern HBRUSH g_hToggleOnBrush;
extern HPEN g_hBorderPen;
extern HPEN g_hAccentPen;

extern HBRUSH g_hEcoBrush;
extern HBRUSH g_hSmartBrush;
extern HBRUSH g_hTurboBrush;
extern HPEN g_hEcoPen;
extern HPEN g_hSmartPen;
extern HPEN g_hTurboPen;

extern UINT g_uTaskbarRestart;
extern NOTIFYICONDATA g_nid;

extern HBITMAP g_hOsdBitmap;
extern HDC g_osdMemDC;
extern HBITMAP g_osdOldBitmap;

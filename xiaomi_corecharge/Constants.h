#pragma once

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

// Settings Controls and IDs
#define ID_GEAR_BUTTON 1100
#define ID_LANG_COMBO 1101
#define ID_AUTOSTART_CHECK 1102
#define ID_MINIMIZE_CHECK 1103
#define ID_SETTINGS_BACK_BTN 1104
#define ID_AUTOSTART_TEXT 1105
#define ID_MINIMIZE_TEXT 1106

#define TIMER_OSD_DISPLAY 100
#define TIMER_OSD_FADE 101

enum Language { LANG_RU = 0, LANG_EN = 1, LANG_ZH = 2 };

// Localization dictionaries
extern const wchar_t *L_AUTO_SAVE_NOTICE[];
extern const wchar_t *L_BATTERY[];
extern const wchar_t *L_CHARGING[];
extern const wchar_t *L_PLUGGED_IN[];
extern const wchar_t *L_LEVEL_TEXT[];
extern const wchar_t *L_BATTERY_CHARGED[];
extern const wchar_t *L_DISCHARGING_SUB[];
extern const wchar_t *L_DISCHARGING_TIME[];
extern const wchar_t *L_CARE_LIMIT_STATUS[];

extern const wchar_t *L_ECO[];
extern const wchar_t *L_SMART[];
extern const wchar_t *L_TURBO[];

extern const wchar_t *L_ECO_DESC[];
extern const wchar_t *L_SMART_DESC[];
extern const wchar_t *L_TURBO_DESC[];

extern const wchar_t *L_CARE_TITLE[];
extern const wchar_t *L_CARE_DESC[];

extern const wchar_t *L_SETTINGS_TITLE[];
extern const wchar_t *L_LANG_LABEL[];
extern const wchar_t *L_AUTOSTART_LABEL[];
extern const wchar_t *L_MINCLOSE_LABEL[];
extern const wchar_t *L_BACK_BTN[];
extern const wchar_t *L_DEMO_MODE[];

extern const wchar_t *L_OSD_TEXTS[][3];

extern const wchar_t *L_TRAY_OPEN[];
extern const wchar_t *L_TRAY_EXIT[];
extern const wchar_t *L_TRAY_CARE_ON[];
extern const wchar_t *L_TRAY_CARE_OFF[];
extern const wchar_t *L_TRAY_MODE[];

extern const wchar_t *L_MSG_ERROR[];
extern const wchar_t *L_MSG_DLL_LOAD_FAIL[];
extern const wchar_t *L_MSG_FUNC_LOAD_FAIL[];
extern const wchar_t *L_MSG_INIT_FAIL[];
extern const wchar_t *L_MSG_REG_CLASS_FAIL[];
extern const wchar_t *L_MSG_CREATE_WINDOW_FAIL[];

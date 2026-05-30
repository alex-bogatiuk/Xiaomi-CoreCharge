#include "Constants.h"

const wchar_t *L_AUTO_SAVE_NOTICE[] = {L"Настройки сохраняются автоматически",
                                       L"Settings are saved automatically",
                                       L"设置会自动保存"};

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
const wchar_t *L_DEMO_MODE[] = {L"Демо-режим", L"Demo Mode", L"演示模式"};

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

#include "ConfigManager.h"
#include "Globals.h"
#include "TaskScheduler.h"
#include <fstream>
#include <string>

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

bool LoadConfig() {
  std::wifstream ifs(CONFIG_FILE);
  if (!ifs)
    return false;
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
  return true;
}

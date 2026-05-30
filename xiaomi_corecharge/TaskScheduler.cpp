#include "TaskScheduler.h"
#include "Utils.h"
#include <stdio.h>

void SetAutostart(bool enable) {
  // Remove legacy registry entry if exists
  HKEY hKey;
  if (RegOpenKeyExW(HKEY_CURRENT_USER,
                    L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0,
                    KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
    RegDeleteValueW(hKey, L"XiaomiCoreCharge");
    RegCloseKey(hKey);
  }

  wchar_t cmdLine[MAX_PATH + 256];
  DWORD exitCode;
  if (enable) {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    // Create the task
    swprintf_s(cmdLine, L"schtasks /create /tn \"XiaomiCoreCharge\" /tr \"\\\"%s\\\" --minimized\" /sc onlogon /rl highest /f", exePath);
    RunCommandHidden(cmdLine, exitCode);

    // Disable battery restrictions and add a 5-second startup delay for the LogonTrigger to ensure SCM is fully ready
    wchar_t psCmd[MAX_PATH + 512];
    swprintf_s(psCmd, L"powershell -Command \"$t = Get-ScheduledTask -TaskName 'XiaomiCoreCharge'; $t.Settings.DisallowStartIfOnBatteries = $false; $t.Settings.StopIfGoingOnBatteries = $false; $t.Triggers[0].Delay = 'PT5S'; Set-ScheduledTask -InputObject $t\"");
    RunCommandHidden(psCmd, exitCode);
  } else {
    swprintf_s(cmdLine, L"schtasks /delete /tn \"XiaomiCoreCharge\" /f");
    RunCommandHidden(cmdLine, exitCode);
  }
}

bool GetAutostart() {
  wchar_t cmdLine[] = L"schtasks /query /tn \"XiaomiCoreCharge\"";
  DWORD exitCode = 1;
  if (RunCommandHidden(cmdLine, exitCode)) {
    return exitCode == 0;
  }
  return false;
}

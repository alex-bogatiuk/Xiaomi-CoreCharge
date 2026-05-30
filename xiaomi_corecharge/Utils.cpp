#include "Utils.h"
#include <string>
#include <algorithm>
#include <cwctype>

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
      // Convert to lowercase for case-insensitive matching
      std::transform(mfg.begin(), mfg.end(), mfg.begin(), ::towlower);
      if (mfg.find(L"xiaomi") != std::wstring::npos ||
          mfg.find(L"timi") != std::wstring::npos ||
          mfg.find(L"tianmi") != std::wstring::npos) {
        isXiaomi = true;
      }
    }
    RegCloseKey(hKey);
  }
  return isXiaomi;
}

bool RunCommandHidden(const wchar_t* cmdLine, DWORD& exitCode) {
  STARTUPINFOW si = { sizeof(si) };
  PROCESS_INFORMATION pi = { 0 };
  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_HIDE;

  wchar_t cmdBuf[1024];
  wcscpy_s(cmdBuf, cmdLine);

  if (CreateProcessW(nullptr, cmdBuf, nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
  }
  return false;
}

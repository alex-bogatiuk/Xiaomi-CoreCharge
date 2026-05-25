# Xiaomi CoreCharge

**Tested on laptops: Redmibook Pro 14 2025 (Ultra 7 255H 32+1T) / Xiaomi Book Pro 14 2026 (Ultra x7 358H 32+1T)**

🚀 **[Download Pre-compiled Release (.zip)](https://github.com/alex-bogatiuk/Xiaomi-CoreCharge/releases/latest)** — run immediately without compiling.

## Description
**Xiaomi CoreCharge** is a system utility for Xiaomi and Redmi laptops designed to manage the hardware Embedded Controller (EC). The program allows limiting the battery charge level, switching CPU performance modes, and provides integration with the Windows system tray and OSD notifications.

The utility interface uses a dark theme with GDI+ vector graphics (Anti-Aliasing) for smoothed controls.

---

## Features

### 🔋 1. Battery Care
* **Hardware Limits**: Toggle battery charge cutoff at the EC controller level.
* **Charge Thresholds**: Pill-shaped buttons to set limits to **50%, 60%, 70%, 80%, 90%, 100%**.
* **State Saving**: Settings are saved automatically when a limit is modified.

### ⚡ 2. Performance Mode Switching
* **Three Power Profiles**:
  * 🌿 **Eco Mode**: Lowers CPU frequencies and power consumption for silent operation.
  * 🧠 **Smart Mode**: Automatically adjusts performance based on the current load.
  * ⚡ **Turbo Mode**: Operates the CPU at the maximum thermal design power (TDP).
* **Color Coding**: Visual highlighting of the active mode.

### 🔔 3. Screen Overlays (OSD Layered Popup)
* Semi-transparent OSD popup window displays when switching modes via buttons, tray menu, or hotkeys.
* **State Rendering**: Smoothed GDI+ icons (Eco leaf, Smart chip, Turbo lightning, battery status) on a gradient background.
* **Fade Effect**: Pixel-by-pixel fade-out sequence using alpha channel transparency (`UpdateLayeredWindow`).

### 🌐 4. Localization
* Full translation of the interface, OSD popups, error messages, and system tray menu into three languages:
  1. **Russian**
  2. **English**
  3. **中国** (Chinese)

### 💻 5. System Integration
* **Autostart**: Writes a parameter to the registry for automatic startup with Windows.
* **Minimize to Tray**: Hides the window to the tray on close for continuous background monitoring.
* **Crisp Icons**: Resource loading via `LoadImage` using system metrics (`SM_CXICON`/`SM_CXSMICON`) to prevent icon blurriness.
* **Flicker-Free Render**: UI drawing utilizing double buffering and `WS_CLIPCHILDREN` style to eliminate button flickering on hover.

---

## Global Hotkeys

* **`Ctrl + Alt + P`**: Cycle through performance modes (`Eco` ➔ `Smart` ➔ `Turbo` ➔ `Eco...`) with OSD popup.
* **`Ctrl + Alt + B`** (Fallback: **`Ctrl + Alt + L`**): Toggle battery care with OSD popup.
  *(Registers 'L' if 'B' is captured by another process)*.

---

## Build & Compile

### Requirements:
* Windows 10 or Windows 11 operating system.
* Visual Studio 2022 / Build Tools 2026 (Toolset **`v145`** or equivalent).
* Hardware port access driver **WinRing0x64.dll / WinRing0.dll** located in the executable folder.

### Command Line Build (MSBuild):
To compile the Release configuration, run this command in a PowerShell terminal:
```powershell
& "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\MSBuild\Current\Bin\MSBuild.exe" xiaomi_corecharge.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v145
```
The compiled executable is saved at: `x64\Release\xiaomi_corecharge.exe`.

---

## Notes
* **Administrator Privileges**: The `WinRing0` driver requires elevated permissions to access hardware I/O ports. Run the application as administrator.
* **Compatibility**: The utility works on devices supporting EC addresses `0x68` (performance) and `0xA4`/`0xA7` (battery). Emulated demonstration mode is active on unsupported hardware.

---

## License & Acknowledgements
* **WinRing0** — hardware low-level access library by [OpenLibSys.org](http://openlibsys.org/) (BSD License).
* **Reference Repository** — developed with reference to the [xiaomi_pc_manager_lite](https://github.com/CHHHHHHEN/xiaomi_pc_manager_lite/releases) project.
* For educational and personal use only. Commercial distribution is prohibited.

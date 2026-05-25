# Guide to Verifying EC Register Compatibility via RWeverything

This guide helps you verify if your Xiaomi/Redmi laptop's Embedded Controller (EC) register offsets and values match the default settings used by **Xiaomi CoreCharge**.

By comparing the official **Xiaomi PC Manager** settings with the real-time values in your laptop's EC memory using **RWeverything**, you can confirm complete hardware compatibility.

---

## Step 1: Download and Run RWeverything
1. Download **RWeverything** (portable version recommended) from the official website: [rweverything.com](http://rweverything.com/download/).
2. Extract the archive and run `RW.exe` (or `RW64.exe` for 64-bit systems) **as Administrator** (right-click -> *Run as administrator*). This is mandatory to allow low-level hardware access.

---

## Step 2: Open the Embedded Controller (EC) Panel
1. In the top toolbar, click the **EC** button (a green chip icon with "EC") or go to the top menu: **Access** -> **Embedded Controller**.
2. A 16x16 grid showing 256 bytes of EC memory (addresses `0x00` to `0xFF`) will open:
   * **Rows** (vertical) are labeled `00`, `10`, `20` ... `A0` ... `F0` (the first hex digit of the address).
   * **Columns** (horizontal) are labeled `0` to `F` (the second hex digit).

---

## Step 3: Verify Battery Care Charging Addresses (`0xA4` and `0xA7`)

We need to verify two addresses:
* **`0xA4`** (Row **A0**, Column **4**) — controls whether the charge limit function is enabled or disabled.
* **`0xA7`** (Row **A0**, Column **7**) — stores the charge limit percentage.

### Test Procedure:
1. Place the RWeverything window and your official **Xiaomi PC Manager** (Battery settings page) side-by-side.
2. Locate the cell at row **A0**, column **7** (`0xA7`) in RWeverything.
3. Change the charge limit in **Xiaomi PC Manager**:
   * Set the limit to **80%**. The value in cell **A7** should become **`50`** (80 in hexadecimal).
   * Set the limit to **60%**. The value in cell **A7** should become **`3C`** (60 in hexadecimal).
   * Set the limit to **100%** (or disable limit). The value in cell **A7** should become **`64`** (100 in hexadecimal).
4. Now locate the cell at row **A0**, column **4** (`0xA4`):
   * **Enable** Battery Care Charging in the official manager. The value in cell **A4** should change so that its lowest bit becomes `1` (commonly, the cell becomes `01` or another odd number like `21`, which is binary `00100001`).
   * **Disable** Battery Care Charging. The value in cell **A4** should change so that its lowest bit becomes `0` (commonly, the cell becomes `20` or `00` — any even number).

> **Result:** If the values in cells **A4** and **A7** change as described above, your laptop is **fully compatible** with the default Battery Care settings.

---

## Step 4: Verify Performance Mode Address (`0x68`)

We need to verify the following address:
* **`0x68`** (Row **60**, Column **8**) — stores the active performance mode.

### Test Procedure:
1. Locate the cell at row **60**, column **8** (`0x68`) in RWeverything.
2. Switch performance modes in the official **Xiaomi PC Manager** (or via the `Fn + K` shortcut) and observe cell **68**:
   * Switch to **Eco Mode** (Энергосберегающий / 省电模式). The value in cell **68** should become **`0A`**.
   * Switch to **Balanced / Smart Mode** (Сбалансированный / 智能模式). The value should become **`09`**.
   * Switch to **Performance / Fast Mode** (Производительный / 极速模式). The value should become **`03`**.

*(Note: If your model supports Extreme/Turbo or Quiet modes, they will show as `04` and `02` respectively.)*

> **Result:** If switching performance modes updates cell **68** to these precise numbers, your laptop's performance mode management is **fully compatible**.

---

## Summary
* **All values match:** Your laptop is 100% compatible. You can safely remove the bloated Xiaomi PC Manager and use this lightweight alternative!
* **Values change, but in different cells:** Your laptop supports EC control, but uses different offsets. You can update the EC address definitions in [main_clean.cpp](file:///c:/Users/user/go/src/xiaomi_corecharge/xiaomi_corecharge/main_clean.cpp) and recompile the project.
* **No values change at all:** Your laptop may use a different interface (like WMI) instead of direct EC registers. Direct EC control is not supported.

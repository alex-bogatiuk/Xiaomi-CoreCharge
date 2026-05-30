#include "OSDController.h"
#include "Constants.h"
#include "Globals.h"
#include <gdiplus.h>
#include <math.h>

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

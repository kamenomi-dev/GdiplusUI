#include "../gdiplusUI.h"


GdiplusUI::Components::Screen::Screen() : Frame() {
  m_hTargetWnd  = NULL;
  m_bFullscreen = false;
}

GdiplusUI::Components::Screen::Screen(Rect rcControl) : Frame(rcControl) {
  m_hTargetWnd  = NULL;
  m_bFullscreen = false;
}

void GdiplusUI::Components::Screen::BindWindow(HWND hWnd) { m_hTargetWnd = hWnd; }

inline void GdiplusUI::Components::Screen::SetFullscreenStatus(bool status) { m_bFullscreen = status; }

inline bool GdiplusUI::Components::Screen::GetFullscreenStatus() const { return m_bFullscreen; }

#include <iostream>
#include <iterator>
#include <random>

LRESULT GdiplusUI::Components::Screen::__MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam) {

  if (uMsg == WM_SIZE) {
    SetFullscreenStatus(wParam == SIZE_MAXIMIZED);
  }

  return this->DefaultMessageHandler(uMsg, wParam, lParam);
}

bool GdiplusUI::Components::Screen::__PaintHandler(Graphics& graphics, void* pRenderInfo) {

  const Rect windowRect = this->GetRect();


  /// Background

  SolidBrush bgBrush(Color(243, 243, 243));

  if (GetFullscreenStatus()) {
    graphics.FillRectangle(&bgBrush, windowRect);
  } else {
    GraphicsPath framePath{};
    GdiplusUI::Utils::GdiplusExt::CreateBezierRoundedRect(framePath, windowRect, 32);

    Pen bgPen(Color(134, 131, 126), 1.5f);

    graphics.FillPath(&bgBrush, &framePath);
    graphics.DrawPath(&bgPen, &framePath);
  }


  /// System control button

  const int buttonCount = 3;
  static unsigned char buttonStatus[3] = {0, 0, 0}; // 存状态，0 最小化 1 最大化 3 关闭 | 0 正常 1 Hover 2 按下

  const Size buttonSize  = LogicManager::GetControlButtonSize();

  Rect controlRect(
      windowRect.Width - buttonSize.Width * buttonCount - 1,
      1,
      buttonSize.Width * buttonCount,
      buttonSize.Height
  );

  const auto savedStatus = graphics.BeginContainer(controlRect, controlRect, UnitPixel);

  SolidBrush bbb(Color::DarkBlue);
  graphics.FillRectangle(&bbb, controlRect);

  graphics.EndContainer(savedStatus);

  return true;
}

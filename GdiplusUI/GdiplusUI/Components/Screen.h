#pragma once
#include "./Frame.h"

#ifndef _GDIPLUSUI_SCREEN_H_
#define _GDIPLUSUI_SCREEN_H_
namespace GdiplusUI {
namespace Components {
class GpUI_API Screen : public Frame {
  public:
  Screen();
  Screen(Rect rcControl);

  void BindWindow(HWND hWnd);

  LRESULT __MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam);
  bool    __PaintHandler(Graphics& graphics);

  private:
  HWND m_hTargetWnd;
};
} // namespace Components
} // namespace GdiplusUI

#endif // !_GDIPLUSUI_SCREEN_H_

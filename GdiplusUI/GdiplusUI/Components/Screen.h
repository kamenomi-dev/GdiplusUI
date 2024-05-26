#pragma once
#include "./Frame.h"

#ifndef _GDIPLUSUI_SCREEN_H_
#define _GDIPLUSUI_SCREEN_H_
namespace GdiplusUI {
namespace Components {
class GpUI_API Screen : virtual public Frame {
  public:
  Screen();
  Screen(Rect rcControl);

  void BindWindow(HWND hWnd);

  inline void SetFullscreenStatus(bool);
  inline bool GetFullscreenStatus() const;

  LRESULT __MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam);
  bool    __PaintHandler(Graphics& graphics, void* reservedData = nullptr);

  private:
  HWND m_hTargetWnd;
  bool m_bFullscreen;
};
} // namespace Components
} // namespace GdiplusUI

#endif // !_GDIPLUSUI_SCREEN_H_

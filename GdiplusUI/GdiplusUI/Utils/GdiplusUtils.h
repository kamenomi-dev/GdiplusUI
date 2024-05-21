#pragma once

#ifndef _GDIPLUSUTILS_H_
#define _GDIPLUSUTILS_H_

namespace GdiplusUI {
namespace Utils {
namespace Gdiplus {
class SwapChain : GdiplusUI::INonCopy {
  public:
  SwapChain();
  ~SwapChain();

  HDC  GetLayoutDC() const;
  void Bind(HWND hWnd);
  void Present();
  void Resize();
  void Resize(LPARAM data);

  HBITMAP __CreateVirtualBitmap(HDC hDC, Size szWnd);

  private:
  HWND m_hTargetWnd;
  Size m_szWnd;

  HDC m_hVirtualDC;
  HBITMAP m_hVirtualBitmap;
  HBITMAP m_hVirtualBitmapOld;
};
} // namespace Gdiplus
} // namespace Utils
} // namespace GdiplusUI

#endif // !_GDIPLUSUTILS_H_

#pragma once

#ifndef _GDIPLUSUTILS_H_
#define _GDIPLUSUTILS_H_

namespace GdiplusUI {
namespace Utils {
namespace GdiplusExt {


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


Color HexToColor(string hex);
Color HexToColor(wstring hex);


Rect RectFToRect(RectF);
RectF RectToRectF(Rect);


void CreateBezierRoundedRect(GraphicsPath& path, RectF rect, float radius);
void CreateBezierRoundedRect(GraphicsPath& path, Rect rect, float radius);

} // namespace GdiplusExt
} // namespace Utils
} // namespace GdiplusUI

#endif // !_GDIPLUSUTILS_H_

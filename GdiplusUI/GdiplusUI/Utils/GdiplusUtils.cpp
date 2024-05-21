#include "../gdiplusUI.h"

GdiplusUI::Utils::Gdiplus::SwapChain::SwapChain() {
  m_hTargetWnd = NULL;
  m_szWnd      = Size();

  m_hVirtualDC        = CreateCompatibleDC(NULL);
  m_hVirtualBitmap    = NULL;
  m_hVirtualBitmapOld = NULL;
}

GdiplusUI::Utils::Gdiplus::SwapChain::~SwapChain() {
  if (m_hVirtualDC) {
    if (m_hVirtualBitmapOld) {
      SelectBitmap(m_hVirtualDC, m_hVirtualBitmapOld);
      DeleteBitmap(m_hVirtualBitmap);

      m_hVirtualBitmap    = NULL;
      m_hVirtualBitmapOld = NULL;
    }

    DeleteDC(m_hVirtualDC);
    m_hVirtualDC = NULL;
  }
}

HDC GdiplusUI::Utils::Gdiplus::SwapChain::GetLayoutDC() const {
  return m_hVirtualDC;
}

void GdiplusUI::Utils::Gdiplus::SwapChain::Bind(HWND hWnd) {
  m_hTargetWnd = hWnd;
}

void GdiplusUI::Utils::Gdiplus::SwapChain::Present() {
  auto targetDC = GetDC(m_hTargetWnd);

  BitBlt(
      targetDC,
      0,
      0,
      m_szWnd.Width,
      m_szWnd.Height,
      GetLayoutDC(),
      0,
      0,
      SRCCOPY
  );

  ReleaseDC(m_hTargetWnd, targetDC);
}

void GdiplusUI::Utils::Gdiplus::SwapChain::Resize() {
  RECT rcWnd{};
  GetWindowRect(m_hTargetWnd, &rcWnd);

  Resize(MAKELPARAM(rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top));
}

void GdiplusUI::Utils::Gdiplus::SwapChain::Resize(LPARAM data) {
  m_szWnd.Width  = GET_X_LPARAM(data);
  m_szWnd.Height = GET_Y_LPARAM(data);

  if (m_hVirtualBitmapOld) {
    SelectObject(m_hVirtualDC, m_hVirtualBitmapOld);
    DeleteObject(m_hVirtualBitmap);
  }

  m_hVirtualBitmap    = __CreateVirtualBitmap(m_hVirtualDC, m_szWnd);
  m_hVirtualBitmapOld = (HBITMAP)SelectObject(m_hVirtualDC, m_hVirtualBitmap);


  Present();
}

HBITMAP GdiplusUI::Utils::Gdiplus::SwapChain::__CreateVirtualBitmap(
    HDC  hDC,
    Size szWnd
) {
  void*      ppvBits{};
  BITMAPINFO bitmapInfo{};
  bitmapInfo.bmiHeader.biBitCount    = 32;
  bitmapInfo.bmiHeader.biCompression = 0;
  bitmapInfo.bmiHeader.biPlanes      = 1;
  bitmapInfo.bmiHeader.biSize        = 40;
  bitmapInfo.bmiHeader.biWidth       = szWnd.Width;
  bitmapInfo.bmiHeader.biHeight      = szWnd.Height;

  return CreateDIBSection(hDC, &bitmapInfo, DIB_RGB_COLORS, &ppvBits, NULL, 0);
}

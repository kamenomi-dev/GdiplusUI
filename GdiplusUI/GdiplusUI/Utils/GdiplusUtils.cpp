#include "../gdiplusUI.h"

GdiplusUI::Utils::GdiplusExt::SwapChain::SwapChain() {
  m_hTargetWnd = NULL;
  m_szWnd      = Size();

  m_hVirtualDC        = CreateCompatibleDC(NULL);
  m_hVirtualBitmap    = NULL;
  m_hVirtualBitmapOld = NULL;
}

GdiplusUI::Utils::GdiplusExt::SwapChain::~SwapChain() {
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

HDC GdiplusUI::Utils::GdiplusExt::SwapChain::GetLayoutDC() const { return m_hVirtualDC; }

void GdiplusUI::Utils::GdiplusExt::SwapChain::Bind(HWND hWnd) { m_hTargetWnd = hWnd; }

void GdiplusUI::Utils::GdiplusExt::SwapChain::Present() {
  auto targetDC = GetDC(m_hTargetWnd);

  BitBlt(targetDC, 0, 0, m_szWnd.Width, m_szWnd.Height, GetLayoutDC(), 0, 0, SRCCOPY);

  ReleaseDC(m_hTargetWnd, targetDC);
}

void GdiplusUI::Utils::GdiplusExt::SwapChain::Resize() {
  RECT rcWnd{};
  GetWindowRect(m_hTargetWnd, &rcWnd);

  Resize(MAKELPARAM(rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top));
}

void GdiplusUI::Utils::GdiplusExt::SwapChain::Resize(LPARAM data) {
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

HBITMAP GdiplusUI::Utils::GdiplusExt::SwapChain::__CreateVirtualBitmap(HDC hDC, Size szWnd) {
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

Color GdiplusUI::Utils::GdiplusExt::HexToColor(string hex) {
  if (*hex.begin() == '#') {
    hex.erase(hex.begin());
  }

  long long convertTemp = std::stoll("0x" + hex, nullptr, 16);

  int channel[4] = {0, 0, 0, 0};
  for (unsigned char i = 0; i < 4; i++) {
    channel[i]    = convertTemp & 255; // FF
    convertTemp >>= 8;
  }

  return Color(channel[3], channel[2], channel[1], channel[0]);
}

Color GdiplusUI::Utils::GdiplusExt::HexToColor(wstring hex) {
  if (*hex.begin() == L'#') {
    hex.erase(hex.begin());
  }

  long long convertTemp = std::stoll(L"0x" + hex, nullptr, 16);

  int channel[4] = {0, 0, 0, 0};
  for (unsigned char i = 0; i < 4; i++) {
    channel[i] = convertTemp & 255; // FF
    convertTemp >>= 8;
  }

  return Color(channel[3], channel[2], channel[1], channel[0]);
}


Rect GdiplusUI::Utils::GdiplusExt::RectFToRect(RectF rect) {
  return Rect((int)rect.X, (int)rect.Y, (int)rect.Width, (int)rect.Height);
}

RectF GdiplusUI::Utils::GdiplusExt::RectToRectF(Rect rect) {
  return RectF(rect.X * 1.f, rect.Y * 1.f, rect.Width * 1.f, rect.Height * 1.f);
}

void GdiplusUI::Utils::GdiplusExt::CreateBezierRoundedRect(GraphicsPath& path, RectF rect, float radius) {
  path.AddBezier(rect.X, rect.Y + radius, rect.X, rect.Y, rect.X, rect.Y, rect.X + radius, rect.Y);

  path.AddBezier(
      rect.GetRight() - radius - 1,
      rect.Y,
      rect.GetRight() - 1,
      rect.Y,
      rect.GetRight() - 1,
      rect.Y,
      rect.GetRight() - 1,
      rect.Y + radius
  );

  path.AddBezier(
      rect.GetRight() - 1,
      rect.GetBottom() - radius - 1,
      rect.GetRight() - 1,
      rect.GetBottom() - 1,
      rect.GetRight() - 1,
      rect.GetBottom() - 1,
      rect.GetRight() - radius - 1,
      rect.GetBottom() - 1
  );

  path.AddBezier(
      rect.X + radius,
      rect.GetBottom() - 1,
      rect.X,
      rect.GetBottom() - 1,
      rect.X,
      rect.GetBottom() - 1,
      rect.X,
      rect.GetBottom() - radius - 1
  );

  path.CloseFigure();
}

void GdiplusUI::Utils::GdiplusExt::CreateBezierRoundedRect(GraphicsPath& path, Rect rect, float radius) {
  path.AddBezier(rect.X, (int)(rect.Y + radius), rect.X, rect.Y, rect.X, rect.Y, (int)(rect.X + radius), rect.Y);

  path.AddBezier(
      (int)(rect.GetRight() - radius) - 1,
      rect.Y,
      rect.GetRight() - 1,
      rect.Y,
      rect.GetRight() - 1,
      rect.Y,
      rect.GetRight() - 1,
      (int)(rect.Y + radius)
  );

  path.AddBezier(
      rect.GetRight() - 1,
      (int)(rect.GetBottom() - radius) - 1,
      rect.GetRight() - 1,
      rect.GetBottom() - 1,
      rect.GetRight() - 1,
      rect.GetBottom(),
      (int)(rect.GetRight() - radius) - 1,
      rect.GetBottom() - 1
  );

  path.AddBezier(
      (int)(rect.X + radius),
      rect.GetBottom() - 1,
      rect.X,
      rect.GetBottom() - 1,
      rect.X,
      rect.GetBottom() - 1,
      rect.X,
      (int)(rect.GetBottom() - radius) - 1
  );

  path.CloseFigure();
}

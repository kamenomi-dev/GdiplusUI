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

LRESULT GdiplusUI::Components::Screen::__MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam) {

  if (uMsg == WM_SIZE) {
    SetFullscreenStatus(wParam == SIZE_MAXIMIZED);
  }

  return this->DefaultMessageHandler(uMsg, wParam, lParam);
}


bool GdiplusUI::Components::Screen::__PaintHandler(Graphics& graphics, void* pRenderInfo) {
  graphics.ResetClip();
  graphics.SetSmoothingMode(SmoothingModeHighQuality);
  const Rect windowRect = this->GetRect();

  // Background
  SolidBrush& bgBrush = ResourceManager::GetBackgroundBrush();
  if (GetFullscreenStatus() || not ResourceManager::IsFrameRounded()) {
    graphics.FillRectangle(&bgBrush, windowRect);
  } else {
    GraphicsPath framePath{};
    GdiplusUI::Utils::GdiplusExt::CreateBezierRoundedRect(framePath, windowRect, 32);

    Pen& bgPen = ResourceManager::GetBackgroundFramePen();
    graphics.FillPath(&bgBrush, &framePath);
    graphics.DrawPath(&bgPen, &framePath);
    graphics.SetClip(&framePath);
  }

  // Icon
  int weight = GDIPLUS_UI_CAPTION_HEIGHT;
  graphics.DrawImage(&ResourceManager::GetWindowIcon(), Rect(4, 4, weight - 8, weight - 8));

  // Title
  graphics.DrawString();

  // System control button status
  // 0: Minimize, 1: Maximize, 2: Close
  ButtonStatus buttonStatus[3] = {ButtonStatus::Invalid, ButtonStatus::Invalid, ButtonStatus::Invalid};
  RIB*         infoBlock       = (RIB*)pRenderInfo;
  if (infoBlock && infoBlock->reservedData) {
    auto statusData    = *(int*)infoBlock->reservedData;
    auto controlID     = LOWORD(statusData);
    auto controlStatus = ButtonStatus(HIWORD(statusData));

    if (controlID == HTMINBUTTON) buttonStatus[0] = controlStatus;
    if (controlID == HTMAXBUTTON) buttonStatus[1] = controlStatus;
    if (controlID == HTCLOSE) buttonStatus[2] = controlStatus;
  }

  // Constants
  const Size        buttonSize    = LogicManager::GetControlButtonSize();
  const Font&       buttonFont    = ResourceManager::GetControlFont();
  const SolidBrush& bgNormalColor = ResourceManager::GetCaptionButtonNormalBrush();
  const SolidBrush& bgHoverColor  = ResourceManager::GetCaptionButtonHoverBrush();
  const SolidBrush& bgDownColor   = ResourceManager::GetCaptionButtonDownBrush();
  const SolidBrush& symbolColor   = ResourceManager::GetControlFontBrush();

  const auto& controlSymbol = ResourceManager::GetControlSymbol();
  
  StringFormat centerFormat{};
  centerFormat.SetAlignment(StringAlignmentCenter);
  centerFormat.SetLineAlignment(StringAlignmentCenter);

  // Drawing buttons
  // Border width is 1 px, so here needs to plus or minus 1px.
  RectF buttonRect(windowRect.Width - buttonSize.Width * 3.f - 1.f, 1.f, buttonSize.Width * 1.f, buttonSize.Height * 1.f);
  graphics.SetSmoothingMode(SmoothingModeHighSpeed);

  for (int i = 0; i < 3; ++i) {
    if (buttonStatus[i] == ButtonStatus::Normal) {
      // graphics.FillRectangle(&bgBrush, buttonRect);
    } else {
      const auto& applyBrush = buttonStatus[i] == ButtonStatus::Hover ? bgHoverColor : bgDownColor;
      graphics.FillRectangle(&applyBrush, buttonRect);
    }

    if (i == 0) {
      graphics.DrawString(controlSymbol.Minimize->c_str(), 1, &buttonFont, buttonRect, &centerFormat, &symbolColor);
    }

    if (i == 1) {
      graphics.DrawString(controlSymbol.Maximize->c_str(), 1, &buttonFont, buttonRect, &centerFormat, &symbolColor);
    }

    if (i == 2) {
      graphics.DrawString(controlSymbol.Close->c_str(), 1, &buttonFont, buttonRect, &centerFormat, &symbolColor);
    }

    buttonRect.X += buttonRect.Width;
  }

  graphics.SetSmoothingMode(SmoothingModeHighQuality);

  return true;
}

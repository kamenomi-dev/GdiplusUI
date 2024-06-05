#include "./gdiplusUI.h"

using namespace GdiplusUI::Utils::Regedit;
using namespace GdiplusUI::Utils::Windows;

GdiplusUI::LogicManager::LogicManager(RenderManager* renderManager) {
  LogicManager::m_instance = this;

  m_posWindow     = {0, 0};
  m_sizeWindow    = {0, 0};
  m_hMessageWnd   = renderManager->GetRenderWindow();
  m_renderManager = renderManager;

  // define default
}

GdiplusUI::LogicManager::~LogicManager() {}

void GdiplusUI::LogicManager::SetWindowTitle(wstring text, bool tryRerender) {
  m_wndTitle = text;
  if (tryRerender) {
    InvalidateRect(m_hMessageWnd, NULL, false);
  }
}

wstring GdiplusUI::LogicManager::GetWindowTitle() { return m_wndTitle; }

void GdiplusUI::LogicManager::SetWindowIcon(Bitmap& icon, bool tryRerender){ 
  HICON hIcon{};
  icon.GetHICON(&hIcon);
  ResourceManager::GetWindowIcon().FromHICON(hIcon);

  if (tryRerender) {
    InvalidateRect(m_hMessageWnd, NULL, false);
  }
}

Bitmap& GdiplusUI::LogicManager::GetWindowIcon() { return ResourceManager::GetWindowIcon(); }

// Todo

int GdiplusUI::LogicManager::GetCaptionHeight() { return GDIPLUS_UI_CAPTION_HEIGHT; }

Size GdiplusUI::LogicManager::GetControlButtonSize() { return Size(GDIPLUS_UI_CONTROL_BUTTON_WDITH, GDIPLUS_UI_CONTROL_BUTTON_HEIGHT); }

void GdiplusUI::LogicManager::UpdateThemeStatus() {
  constexpr auto registryPath = L"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
  constexpr auto registryKey  = L"AppsUseLightTheme";

  Regedit reg(registryPath);
  auto    isDarkMode = reg.GetItem<DWORD>(registryKey) == 0;

  WindowLayer::SetDarkMode(m_hMessageWnd, isDarkMode);
}


void GdiplusUI::LogicManager::CaptionRenderProcess(UINT uMsg, LRESULT result) {

  ButtonStatus buttonStatus = ButtonStatus::Normal;
  if (uMsg == WM_NCHITTEST) {
    buttonStatus = ButtonStatus::Hover;
  } else if (uMsg == WM_NCLBUTTONDOWN) {
    buttonStatus = ButtonStatus::Down;
  }


  auto     swapChain = m_renderManager->GetSwapContext();
  Graphics graphics(swapChain->GetLayoutDC());

  int statusData = MAKELONG(result, buttonStatus);

  RIB infoBlock{};
  infoBlock.flag         = RenderFlag::CaptionRender;
  infoBlock.graphics     = &graphics;
  infoBlock.reservedData = &statusData;


  const auto lastSmoothMode = graphics.GetSmoothingMode();
  graphics.SetSmoothingMode(SmoothingModeDefault);

  m_renderManager->GetControlRoot()->DefaultMessageHandler(WM_PAINT, result, reinterpret_cast<LPARAM>(&infoBlock));

  graphics.SetSmoothingMode(lastSmoothMode);
  swapChain->Present();
}


LRESULT GdiplusUI::LogicManager::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  static unsigned int themeColor = 0; // AARRGGBB
  static SwapChain*   swapChain;      // Need to check nullpointer.

  LRESULT lResult{};
  if (WindowMessageHandler(hWnd, uMsg, wParam, lParam, lResult)) {
    return lResult;
  }

  if (uMsg == WM_DWMCOLORIZATIONCOLORCHANGED) {

    if (uMsg == themeColor) {
      return 0;
    }

    themeColor = (unsigned int)wParam;
    UpdateThemeStatus();
  }


  if (uMsg == WM_CREATE) {
    UpdateThemeStatus();

    GdiplusUI::Utils::Windows::WindowLayer::SetBlurEffect(hWnd, Utils::Windows::WindowLayer::Aero);


    swapChain = m_renderManager->GetSwapContext();


    // Fix the smooth sizing effect.

    const auto exStyle = GetWindowLongW(hWnd, GWL_EXSTYLE);
    SetWindowLongW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_NOREDIRECTIONBITMAP);

    const auto style = GetWindowLongW(hWnd, GWL_STYLE);
    SetWindowLongW(hWnd, GWL_STYLE, style & ~WS_SYSMENU);
  }


  if (uMsg == WM_MOVE) {
    m_posWindow.x = GET_X_LPARAM(lParam);
    m_posWindow.y = GET_Y_LPARAM(lParam);
  }


  if (uMsg == WM_SIZE) {
    assert(swapChain);
    swapChain->Resize(lParam);
    m_sizeWindow.cx = GET_X_LPARAM(lParam);
    m_sizeWindow.cy = GET_Y_LPARAM(lParam);
    {

      //  Todo  Correctly event handler!

      PostMessageEventToAll(uMsg, wParam, lParam);
    }
    return 0;
  }


  if (uMsg == WM_PAINT) {
    assert(swapChain);

    Graphics presentGrap(swapChain->GetLayoutDC());
    presentGrap.Clear(Color::Transparent);
    presentGrap.SetSmoothingMode(SmoothingModeHighQuality);

    RIB infoBlock{};
    infoBlock.flag         = RenderFlag::NormalRender;
    infoBlock.graphics     = &presentGrap;
    infoBlock.reservedData = nullptr;

    PostMessageEventToAllEx(WM_PAINT, NULL, (LPARAM)&infoBlock, [](PostMsgCbDataStruct data) -> void {
      if (data.lastTarget == nullptr) {
        return;
      }

      auto lastControl = data.lastTarget;
      auto presentGrap = RenderManager::GetGraphics(data.lParam);
      presentGrap->ExcludeClip(lastControl->GetRect());
    });

    swapChain->Present();
  }


  if (uMsg == WM_ERASEBKGND) {
    return 0;
  }


  LRESULT extFrameResult;
  if (ExtendFrameMessageHandler(hWnd, uMsg, wParam, lParam, extFrameResult)) {
    return extFrameResult;
  }


  return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

bool GdiplusUI::LogicManager::WindowMessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) {
  if (uMsg == WM_SETTEXT) {
    SetWindowTitle((wchar_t*)lParam, true);
  }
  return false; }


bool GdiplusUI::LogicManager::CaptionMessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) {
  static bool needParse = false;
  LRESULT     result{};

  Point screenPoint{};
  screenPoint.X = GET_X_LPARAM(lParam) - m_posWindow.x;
  screenPoint.Y = GET_Y_LPARAM(lParam) - m_posWindow.y;
  // ScreenToClient(hWnd, &screenPoint);

  const auto btnSize  = LogicManager::GetControlButtonSize();
  const auto wndWidth = m_sizeWindow.cx - 1;

  const int btnHeight    = btnSize.Height + 1;
  const int minBtnLeft   = wndWidth - btnSize.Width * 3;
  const int maxBtnLeft   = wndWidth - btnSize.Width * 2;
  const int closeBtnLeft = wndWidth - btnSize.Width;
  const int rightBound   = wndWidth;

  if ((screenPoint.Y > btnHeight || screenPoint.Y <= 1)) {
    if (needParse) {
      needParse = false;
      CaptionRenderProcess(NULL); // Reset status
    }
    return false;
  }

  if (screenPoint.X < minBtnLeft and needParse) {
    if (needParse) {
      needParse = false;
      CaptionRenderProcess(NULL); // Reset status
    }
    return false;
  }

  if (minBtnLeft <= screenPoint.X && screenPoint.X < maxBtnLeft) {
    result = HTMINBUTTON;
  } else if (maxBtnLeft <= screenPoint.X && screenPoint.X < closeBtnLeft) {
    result = HTMAXBUTTON;
  } else if (closeBtnLeft <= screenPoint.X && screenPoint.X < rightBound) {
    result = HTCLOSE;
  }

  if (result != NULL) {
    CaptionRenderProcess(WM_NCHITTEST, result);
    lResult   = result;
    needParse = true;
    return true;
  }

  if (needParse) {
    needParse = false;
    CaptionRenderProcess(NULL); // Reset status
  }
  return false;
}


bool GdiplusUI::LogicManager::ExtendFrameMessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) {
  static MARGINS margins          = {0, 0, LogicManager::GetControlButtonSize().Height + 1, 0};
  static RECT    border_thickness = {0};


  if (uMsg == WM_CREATE) {
    if (GetWindowLongW(hWnd, GWL_STYLE) & WS_THICKFRAME) {

      AdjustWindowRectEx(&border_thickness, GetWindowLongW(hWnd, GWL_STYLE) & ~WS_CAPTION, FALSE, NULL);
      border_thickness.left *= -1;
      border_thickness.top  *= -1;

    } else if (GetWindowLongW(hWnd, GWL_STYLE) & WS_BORDER) {

      border_thickness = {1, 1, 1, 1};
    }

    DwmExtendFrameIntoClientArea(hWnd, &margins);
    SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
  }

  if (uMsg == WM_NCCALCSIZE) {

    if (lParam) {

      NCCALCSIZE_PARAMS* sz = (NCCALCSIZE_PARAMS*)lParam;
      // sz->rgrc[0].top       += border_thickness.top;
      sz->rgrc[0].left   += border_thickness.left;
      sz->rgrc[0].right  -= border_thickness.right;
      sz->rgrc[0].bottom -= border_thickness.bottom;
      return true;
    }
  }

  if (uMsg == WM_NCHITTEST) {

    lResult = DefWindowProcW(hWnd, uMsg, wParam, lParam);

    if (CaptionMessageHandler(hWnd, uMsg, wParam, lParam, lResult)) {
      return true;
    }

    if (lResult == HTCLIENT) {
      int mouseY = GET_Y_LPARAM(lParam) - m_posWindow.y;

      if (mouseY < border_thickness.top) {
        lResult = HTTOP;
      } else if (mouseY < margins.cyTopHeight) {
        lResult = HTCAPTION;
      };
    }

    return true;
  }

  if (uMsg == WM_NCMOUSEMOVE || uMsg == WM_MOUSEMOVE) {
    if (uMsg == WM_MOUSEMOVE) {
      POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
      ClientToScreen(hWnd, &pt);
      lParam = MAKELPARAM(pt.x, pt.y);
    }

    LRESULT noop{};
    if (not CaptionMessageHandler(hWnd, uMsg, wParam, lParam, noop)) {
      CaptionRenderProcess(NULL); // Reset status
    }
  }

  if (uMsg == WM_NCLBUTTONDOWN) {

    LRESULT result{};
    if (CaptionMessageHandler(hWnd, uMsg, wParam, lParam, result)) {
      CaptionRenderProcess(uMsg, result);
      lResult = NULL;
      return true;
    }
  }

  if (uMsg == WM_NCLBUTTONUP) {

    LRESULT result{};
    if (CaptionMessageHandler(hWnd, uMsg, wParam, lParam, result)) {

      if (result == HTMINBUTTON) {
        ShowWindow(hWnd, SW_MINIMIZE);
      }

      if (result == HTMAXBUTTON) {
        if (IsZoomed(hWnd)) {
          ShowWindow(hWnd, SW_NORMAL);
        } else {
          ShowWindow(hWnd, SW_MAXIMIZE);
        }
      }

      if (result == HTCLOSE) {
        DestroyWindow(hWnd);
      }

      CaptionRenderProcess(NULL); // Reset status
      lResult = NULL;
      return true;
    }
  }


  return false;
}


/// <summary>
/// Call All the children controls of root(screen) control. (including screen.)
/// </summary>
void GdiplusUI::LogicManager::PostMessageEventToAll(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  const auto root = m_renderManager->GetControlRoot();
  root->__MessageHandler(uMsg, wParam, lParam);
  _PostMessageEventToAll(root, uMsg, wParam, lParam);
}

void GdiplusUI::LogicManager::PostMessageEventToAllEx(UINT uMsg, WPARAM wParam, LPARAM lParam, PostMessageCallback proc) {
  const auto root = m_renderManager->GetControlRoot();
  root->__MessageHandler(uMsg, wParam, lParam);
  _PostMessageEventToAllEx(root, uMsg, wParam, lParam, proc);
}

void GdiplusUI::LogicManager::PostMessageEventToTarget(Control* target, UINT uMsg, WPARAM wParam, LPARAM lParam, bool enableBubble) {
  _PostMessageEventToTarget(target, uMsg, wParam, lParam, enableBubble);
}

/// <summary>
/// Call All the children controls of parent control. (including parent.)
/// </summary>
void GdiplusUI::LogicManager::_PostMessageEventToAll(Control* parent, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  auto& children = parent->GetChildren();
  for (auto item : children) {
    _PostMessageEventToAll(item, uMsg, wParam, lParam);
  }

  parent->__MessageHandler(uMsg, wParam, lParam);
}

void GdiplusUI::LogicManager::_PostMessageEventToAllEx(Control* parent, UINT uMsg, WPARAM wParam, LPARAM lParam, PostMessageCallback proc) {
  PostMsgCbDataStruct dataStruct{};
  dataStruct.lastTarget = nullptr;
  dataStruct.uMsg       = uMsg;
  dataStruct.wParam     = wParam;
  dataStruct.lParam     = lParam;

  auto& children = parent->GetChildren();

  for (auto item : children) {
    dataStruct.target = item;

    proc(dataStruct);
    _PostMessageEventToAll(item, uMsg, wParam, lParam);

    dataStruct.lastTarget = item;
  }

  parent->__MessageHandler(uMsg, wParam, lParam);
}

/// <summary>
/// Todo. Let it supports the target controls which is targeted is saved in
/// argument.
/// </summary>
void GdiplusUI::LogicManager::_PostMessageEventToTarget(Control* target, UINT uMsg, WPARAM wParam, LPARAM lParam, bool enableBubble) {
  target->__MessageHandler(uMsg, wParam, lParam);

  if (not enableBubble) {
    return;
  }

  auto parent = target->GetParent();
  if (parent) {
    _PostMessageEventToTarget(parent, uMsg, wParam, lParam, enableBubble);
  };
}

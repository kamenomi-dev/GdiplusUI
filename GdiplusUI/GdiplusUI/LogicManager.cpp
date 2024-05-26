#include "./gdiplusUI.h"

GdiplusUI::LogicManager::LogicManager(RenderManager* renderManager) {
  m_hMessageWnd   = renderManager->GetRenderWindow();
  m_renderManager = renderManager;

  // define default
}

GdiplusUI::LogicManager::~LogicManager() {}

// Todo
Size GdiplusUI::LogicManager::GetControlButtonSize() { return Size(60, 44); }

void GdiplusUI::LogicManager::UpdateThemeStatus() {
  GdiplusUI::Utils::Regedit::Regedit reg(L"HKEY_CURRENT_"
                                         L"USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personali"
                                         L"ze");
  GdiplusUI::Utils::Windows::WindowLayer::SetDarkMode(m_hMessageWnd, reg.GetItem<DWORD>(L"AppsUseLightTheme") == 0);
}

void GdiplusUI::LogicManager::WindowCaptionRender(UINT uMsg, LRESULT result) {

  if (uMsg == WM_NCHITTEST) {

    auto     swapChain = m_renderManager->GetSwapContext();
    Graphics presentGrap(swapChain->GetLayoutDC());
    presentGrap.SetSmoothingMode(SmoothingModeHighQuality);

    RIB infoBlock{};
    infoBlock.flag = RenderFlagCaptionRender;
    infoBlock.graphics = &presentGrap;
    infoBlock.reservedData = &result;

    m_renderManager->GetControlRoot()->DefaultMessageHandler(WM_PAINT, result, (LPARAM) &infoBlock);

    swapChain->Present();

  }

}


LRESULT GdiplusUI::LogicManager::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  static unsigned int themeColor = 0; // AARRGGBB
  static SwapChain*   swapChain;      // Need to check nullpointer.


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


  if (uMsg == WM_SIZE) {
    assert(swapChain);
    swapChain->Resize(lParam);
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
    infoBlock.flag = RenderFlagNormalRender;
    infoBlock.graphics = &presentGrap;
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


bool GdiplusUI::LogicManager::WindowCaptionHittest(HWND hWnd, WPARAM wParam, LPARAM lParam, LRESULT& lResult) {

  POINT screenPoint{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
  ScreenToClient(hWnd, &screenPoint);

  const auto& btnSize = LogicManager::GetControlButtonSize();
  auto        wndRect = m_renderManager->GetControlRoot()->GetRect();

  // screenPoint.x = screenPoint.x - btnSize.Width * 3 + 1;

  if (screenPoint.y > btnSize.Height + 1) {
    return false;
  }


  if (wndRect.Width - btnSize.Width * 3 - 1 <= screenPoint.x && screenPoint.x < wndRect.Width - btnSize.Width * 2 - 1) {
    lResult = HTMINBUTTON;
    WindowCaptionRender(WM_NCHITTEST, lResult);
    return true;
  }

  if (wndRect.Width - btnSize.Width * 2 - 1 <= screenPoint.x && screenPoint.x < wndRect.Width - btnSize.Width - 1) {
    lResult = HTMAXBUTTON;
    WindowCaptionRender(WM_NCHITTEST, lResult);
    return true;
  }

  if (wndRect.Width - btnSize.Width - 1 <= screenPoint.x && screenPoint.x < wndRect.Width - 1) {
    lResult = HTCLOSE;
    WindowCaptionRender(WM_NCHITTEST, lResult);
    return true;
  }

  return false;
}

bool GdiplusUI::LogicManager::ExtendFrameMessageHandler(
    HWND     hWnd,
    UINT     uMsg,
    WPARAM   wParam,
    LPARAM   lParam,
    LRESULT& lResult
) {
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


  if (uMsg == WM_NCHITTEST) {

    lResult = DefWindowProcW(hWnd, uMsg, wParam, lParam);

    if (WindowCaptionHittest(hWnd, wParam, lParam, lResult)) {
      return true;
    }

    if (lResult == HTCLIENT) {

      POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
      ScreenToClient(hWnd, &pt);

      if (pt.y < border_thickness.top) lResult = HTTOP;
      else if (pt.y < margins.cyTopHeight) lResult = HTCAPTION;
    }

    return true;
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


  // Skip the event of mouse from control button.
  if (uMsg == WM_NCLBUTTONDOWN) {

    LRESULT result{};
    if (WindowCaptionHittest(hWnd, wParam, lParam, result)) {
      WindowCaptionRender(uMsg, result);
      lResult = NULL;
      return true;
    }
  }

  if (uMsg == WM_NCLBUTTONUP) {

    LRESULT result{};
    if (WindowCaptionHittest(hWnd, wParam, lParam, result)) {

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

      WindowCaptionRender(uMsg, result);
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

void GdiplusUI::LogicManager::PostMessageEventToAllEx(
    UINT                uMsg,
    WPARAM              wParam,
    LPARAM              lParam,
    PostMessageCallback proc
) {
  const auto root = m_renderManager->GetControlRoot();
  root->__MessageHandler(uMsg, wParam, lParam);
  _PostMessageEventToAllEx(root, uMsg, wParam, lParam, proc);
}

void GdiplusUI::LogicManager::PostMessageEventToTarget(
    Control* target,
    UINT     uMsg,
    WPARAM   wParam,
    LPARAM   lParam,
    bool     enableBubble
) {
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

void GdiplusUI::LogicManager::_PostMessageEventToAllEx(
    Control*            parent,
    UINT                uMsg,
    WPARAM              wParam,
    LPARAM              lParam,
    PostMessageCallback proc
) {
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
void GdiplusUI::LogicManager::_PostMessageEventToTarget(
    Control* target,
    UINT     uMsg,
    WPARAM   wParam,
    LPARAM   lParam,
    bool     enableBubble
) {
  target->__MessageHandler(uMsg, wParam, lParam);

  if (not enableBubble) {
    return;
  }

  auto parent = target->GetParent();
  if (parent) {
    _PostMessageEventToTarget(parent, uMsg, wParam, lParam, enableBubble);
  };
}

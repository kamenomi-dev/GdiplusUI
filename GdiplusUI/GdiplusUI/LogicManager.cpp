#include "./gdiplusUI.h"

GdiplusUI::LogicManager::LogicManager(RenderManager* renderManager) {
  m_hMessageWnd   = renderManager->GetRenderWindow();
  m_renderManager = renderManager;
}

GdiplusUI::LogicManager::~LogicManager() {}


LRESULT GdiplusUI::LogicManager::MessageHandler(
    HWND   hWnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
) {
  static SwapChain* swapChain; // Need to check nullpointer.

  if (uMsg == WM_CREATE) {
    GdiplusUI::Utils::Windows::WindowLayer::EnableBlurEffect(hWnd, Utils::Windows::WindowLayer::Aero);
    swapChain = m_renderManager->GetSwapContext();
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


  // Handle WM_PAINT
  if (uMsg == WM_PAINT) {
    assert(swapChain);

    Graphics presentGrap(swapChain->GetLayoutDC());
    auto     ret = presentGrap.GetLastStatus();
    presentGrap.Clear(Color(255 * 0.1, 255, 255, 255)); // Todo.

    PostMessageEventToAllEx(
        WM_PAINT,
        NULL,
        (LPARAM)&presentGrap,
        [](PostMsgCbDataStruct data) -> void {
          if (data.lastTarget == nullptr) {
            return;
          }

          auto lastControl = data.lastTarget;
          auto presentGrap = RenderManager::GetGraphics(data.lParam);
          presentGrap->ExcludeClip(lastControl->GetRect());
        }
    );

    swapChain->Present();
  }


  if (uMsg == WM_ERASEBKGND) {
    return 0;
  }


  return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

/// <summary>
/// Call All the children controls of root(screen) control. (including screen.)
/// </summary>
void GdiplusUI::LogicManager::PostMessageEventToAll(
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
) {
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
void GdiplusUI::LogicManager::_PostMessageEventToAll(
    Control* parent,
    UINT     uMsg,
    WPARAM   wParam,
    LPARAM   lParam
) {
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

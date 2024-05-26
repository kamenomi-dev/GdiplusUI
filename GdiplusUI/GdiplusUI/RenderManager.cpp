#include "./gdiplusUI.h"

GdiplusUI::RenderManager::RenderManager() {
  m_renderWindow = NULL;
  m_controlRoot  = nullptr;

  m_swapChain = new SwapChain;
}

GdiplusUI::RenderManager::RenderManager(Control* screen, HWND hWnd) {
  m_renderWindow = hWnd;
  m_controlRoot  = screen;
  m_swapChain    = new Utils::GdiplusExt::SwapChain;

  m_swapChain->Bind(hWnd);
}

GdiplusUI::RenderManager::~RenderManager() {
  delete m_swapChain;
  m_swapChain = nullptr;
}

/// <summary>
/// Put this function into __PaintHandler function block.
/// </summary>
Graphics* GdiplusUI::RenderManager::GetGraphics(LPARAM data) {
  // 抽象一下，方便组件开发
  return ((RIB*)data)->graphics;
}


Control* GdiplusUI::RenderManager::GetControlRoot() const {
  return m_controlRoot;
}

SwapChain* GdiplusUI::RenderManager::GetSwapContext() const { return m_swapChain; }

HWND GdiplusUI::RenderManager::GetRenderWindow() const {
  return m_renderWindow;
}

template <typename fn>
Control*
GdiplusUI::RenderManager::__FindControl(fn const& logicFunc, Control* parent) {
  assert(m_controlRoot);

  if (parent == nullptr) {
    parent = m_controlRoot;
  }

  for (auto item : *(parent->m_children)) {
    if (logicFunc(item)) {
      auto ret = (Control*)__FindControl(logicFunc, item);
      return ret != nullptr ? ret : item;
    }
  }

  return nullptr;
}

Control* GdiplusUI::RenderManager::FindControl(Point posPoint) {
  auto ret = __FindControl([posPoint](Control* target) -> bool {
    return target->GetRect().Contains(posPoint);
  });

  return ret != nullptr ? ret : m_controlRoot;
}

Control* GdiplusUI::RenderManager::FindControl(wstring idName) {
  auto ret = __FindControl([idName](Control* target) -> bool {
    return target->GetControlID() == idName;
  });

  return ret != nullptr ? ret : m_controlRoot;
}

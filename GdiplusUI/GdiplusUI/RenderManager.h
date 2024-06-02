#pragma once

#ifndef _RENDERMANAGER_H_
#define _RENDERMANAGER_H_


using namespace GdiplusUI::Components;
using GdiplusUI::Utils::GdiplusExt::SwapChain;

namespace GdiplusUI {

// #define __FindControlCallback [](Control*)->bool;

namespace Defines {

enum class RenderFlag : unsigned char { NormalRender, CaptionRender };

enum class ButtonStatus : unsigned char {
  Invalid = 255U,
  Reset   = 255U,
  Normal  = 255U,

  Hover = 0,
  Down
};

typedef struct {
  RenderFlag flag;
  Graphics*  graphics;
  void*      reservedData;
} RenderInformationBlock, RIB;
}; // namespace Defines


using namespace Defines;

class GpUI_API RenderManager : public INonCopy {
  public:
  RenderManager();
  RenderManager(Control* screen, HWND hWnd, ResourceManager* resourceManager); // control root.
  ~RenderManager();

  static Graphics* GetGraphics(LPARAM data);

  Control*   GetControlRoot() const;
  SwapChain* GetSwapContext() const;
  HWND       GetRenderWindow() const;

  template <typename fn>
  Control* __FindControl(fn const& logicFunc, Control* parent = nullptr);
  Control* FindControl(Point posPoint);
  Control* FindControl(wstring idName);

  private:
  HWND     m_renderWindow;
  Control* m_controlRoot;

  SwapChain*       m_swapChain;
  ResourceManager* m_resourceManager;
};
} // namespace GdiplusUI

#endif // !_RENDERMANAGER_H_
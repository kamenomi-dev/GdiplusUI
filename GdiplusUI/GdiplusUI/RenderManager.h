#pragma once

#ifndef _RENDERMANAGER_H_
#define _RENDERMANAGER_H_


using namespace GdiplusUI::Components;
using GdiplusUI::Utils::GdiplusExt::SwapChain;

namespace GdiplusUI {

// #define __FindControlCallback [](Control*)->bool;

namespace Defines {

typedef enum { RenderFlagNormalRender, RenderFlagCaptionRender } RenderFlag;

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
  RenderManager(Control* screen, HWND hWnd); // control root.
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

  SwapChain* m_swapChain;
};
} // namespace GdiplusUI

#endif // !_RENDERMANAGER_H_
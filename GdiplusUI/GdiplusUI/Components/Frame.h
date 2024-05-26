#pragma once
#include "./Control.h"

#ifndef _GDIPLUSUI_FRAME_H_
#define _GDIPLUSUI_FRAME_H_

namespace GdiplusUI {
namespace Components {
class GpUI_API Frame : public Control {
  public:
  Frame();
  Frame(Rect rcControl);
  virtual ~Frame() = default;

  bool    __PaintHandler(const Graphics& graphics, void* reservedData = nullptr);
  LRESULT __MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam);

  virtual void              SetParent(Control* parent);
  virtual Control*          GetParent();
  virtual void              SetChild(Control* child);
  virtual vector<Control*>& GetChildren();
};
} // namespace Components
} // namespace GdiplusUI


#endif // !_GDIPLUSUI_FRAME_H_

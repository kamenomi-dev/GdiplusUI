#pragma once
// #include "../gdiplusUI.h"


#ifndef _GDIPLUSUI_CONTROL_H_
#define _GDIPLUSUI_CONTROL_H_

namespace GdiplusUI {
namespace Components {
class GpUI_API Control : public INonCopy {

  /// <summary>
  /// Methods
  /// </summary>

  public:
  Control();
  Control(Rect rcControl);
  virtual ~Control();

  wstring GetControlID() const;

  virtual void              SetParent(Control* parent);
  virtual Control*          GetParent();
  virtual void              SetChild(Control* child);
  virtual vector<Control*>& GetChildren();

  Rect GetRect() const;
  void SetRect(Rect rcObject);
  bool IsVisible() const;
  void SetVisible(bool status, bool force = false);

  LRESULT DefaultMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam);

  virtual LRESULT __MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual bool    __PaintHandler(Graphics&, void* reservedData = nullptr);


  /// <summary>
  /// Numbers
  /// </summary>

  public:
  Control*          m_parent;
  vector<Control*>* m_children;

  private:
  wstring m_controlID;

  bool m_bVisible;
  Rect m_rcControl;
};
} // namespace Components
} // namespace GdiplusUI

#endif // !_GDIPLUSUI_CONTROL_H_

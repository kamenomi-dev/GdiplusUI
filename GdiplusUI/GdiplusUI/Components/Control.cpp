#include "../gdiplusUI.h"

using namespace GdiplusUI::Components;

GdiplusUI::Components::Control::Control() {
  m_bVisible  = false;
  m_rcControl = Rect();
  m_parent    = nullptr;
  m_children  = new vector<Control*>();
}

GdiplusUI::Components::Control::Control(Rect rcControl) {
  m_bVisible  = false;
  m_rcControl = Rect(rcControl);
  m_parent    = nullptr;
  m_children  = new vector<Control*>();
}

GdiplusUI::Components::Control::~Control() {
  delete m_children;
  m_children = nullptr;
}

wstring GdiplusUI::Components::Control::GetControlID() const {
  return m_controlID;
}

void GdiplusUI::Components::Control::SetParent(Control* parent) {
  if (m_parent != nullptr) {
    auto& childrenList = m_parent->m_children;
    auto  target = std::find(childrenList->begin(), childrenList->end(), this);

    if (target != childrenList->end()) {
      childrenList->erase(target);
    }

    m_parent = nullptr;
  }

  m_parent = parent;
  parent->m_children->push_back(this);
}

Control* GdiplusUI::Components::Control::GetParent() { return m_parent; }

void GdiplusUI::Components::Control::SetChild(Control* child) {}

vector<Control*>& GdiplusUI::Components::Control::GetChildren() {
  return *m_children;
}

Rect GdiplusUI::Components::Control::GetRect() const { return m_rcControl; }

void GdiplusUI::Components::Control::SetRect(Rect rcObject) {
  m_rcControl = rcObject;
}

bool GdiplusUI::Components::Control::IsVisible() const { return m_bVisible; }

void GdiplusUI::Components::Control::SetVisible(bool status, bool force) {
  ///
  /// TODO    Support force mode.
  ///
  m_bVisible = status;
}

LRESULT GdiplusUI::Components::Control::DefaultMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam) {

  if (uMsg == WM_SIZE) {
    SetRect({0, 0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)});
  }

  if (uMsg == WM_PAINT) {
    return __PaintHandler(*RenderManager::GetGraphics(lParam), (void*)lParam);
  }

  return 0;
}

LRESULT GdiplusUI::Components::Control::__MessageHandler(
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
) {
  return DefaultMessageHandler(uMsg, wParam, lParam);
}

bool GdiplusUI::Components::Control::__PaintHandler(Graphics& graphics, void* reservedData) {
  return false;
}

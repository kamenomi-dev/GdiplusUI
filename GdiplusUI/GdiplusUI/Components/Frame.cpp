#include "../gdiplusUI.h"

using namespace GdiplusUI::Components;

GdiplusUI::Components::Frame::Frame() : Control() {}

GdiplusUI::Components::Frame::Frame(Rect rcControl) : Control(rcControl) {}


bool GdiplusUI::Components::Frame::__PaintHandler(const Graphics& graphics, void* reservedData) { return true; }

LRESULT GdiplusUI::Components::Frame::__MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam) { return 0; }

void GdiplusUI::Components::Frame::SetParent(Control* parent) {
  if (m_parent != nullptr) {
    auto& childrenList = m_parent->m_children;
    auto  target       = std::find(childrenList->begin(), childrenList->end(), this);

    if (target != childrenList->end()) {
      childrenList->erase(target);
    }

    m_parent = nullptr;
  }

  m_parent = parent;
  parent->m_children->push_back(this);
}

Control* GdiplusUI::Components::Frame::GetParent() { return m_parent; }

void GdiplusUI::Components::Frame::SetChild(Control* child) {
  if (std::find(m_children->begin(), m_children->end(), child) == m_children->end()) {
    return;
  }

  child->SetParent(this);
}

vector<Control*>& GdiplusUI::Components::Frame::GetChildren() { return *m_children; }
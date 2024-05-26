#include "./gdiplusUI.h"

GdiplusUI::ResourceManager::ResourceManager() { m_controlFont = new Font(L"Segoe MDL2 Assets", 16); }

GdiplusUI::ResourceManager::~ResourceManager() {
  if (m_controlFont != nullptr) {
    delete m_controlFont;
    m_controlFont = nullptr;
  }
}

Font& GdiplusUI::ResourceManager::GetControlFont() { return *m_controlFont; }

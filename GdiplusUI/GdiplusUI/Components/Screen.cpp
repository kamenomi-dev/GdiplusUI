#include "../gdiplusUI.h"


GdiplusUI::Components::Screen::Screen() : Frame() { m_hTargetWnd = NULL; }

GdiplusUI::Components::Screen::Screen(Rect rcControl) : Frame(rcControl) {
  m_hTargetWnd = NULL;
}

void GdiplusUI::Components::Screen::BindWindow(HWND hWnd) {
  m_hTargetWnd = hWnd;
}

#include <iostream>
#include <iterator>
#include <random>

LRESULT GdiplusUI::Components::Screen::__MessageHandler(
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
) {


  return ((Control*)this)->__MessageHandler(uMsg, wParam, lParam);
}

bool GdiplusUI::Components::Screen::__PaintHandler(Graphics& graphics) {

  return true;
}

#pragma once

#ifndef _LOGICMANAGER_H_
#define _LOGICMANAGER_H_

namespace GdiplusUI {

typedef struct __tagPostMessageCallbackDataStruct {
  Control* lastTarget;
  Control* target;
  UINT     uMsg;
  WPARAM   wParam;
  LPARAM   lParam;
} PostMsgCbDataStruct;
typedef void(__stdcall PostMessageCallback)(PostMsgCbDataStruct);

class LogicManager : INonCopy {
  public:
  LogicManager(RenderManager* renderManager);
  ~LogicManager();


  static Size GetControlButtonSize();

  void UpdateThemeStatus();
  void CaptionRenderProcess(UINT uMsg, LRESULT result = -1);

  LRESULT MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  bool    CaptionMessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
  bool    ExtendFrameMessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

  void PostMessageEventToAll(UINT uMsg, WPARAM wParam, LPARAM lParam);
  void PostMessageEventToAllEx(UINT uMsg, WPARAM wParam, LPARAM lParam, PostMessageCallback proc);
  void PostMessageEventToTarget(Control* target, UINT uMsg, WPARAM wParam, LPARAM lParam, bool enableBubble = false);

  private:
  void _PostMessageEventToAll(Control* parent, UINT uMsg, WPARAM wParam, LPARAM lParam);
  void _PostMessageEventToAllEx(Control* parent, UINT uMsg, WPARAM wParam, LPARAM lParam, PostMessageCallback proc);
  void _PostMessageEventToTarget(Control* target, UINT uMsg, WPARAM wParam, LPARAM lParam, bool enableBubble);

  private:
  POINT          m_posWindow;
  SIZE           m_sizeWindow;
  HWND           m_hMessageWnd;
  RenderManager* m_renderManager;
};
} // namespace GdiplusUI

#endif // !_GDIPLUS_LOGICMANAGER_H_

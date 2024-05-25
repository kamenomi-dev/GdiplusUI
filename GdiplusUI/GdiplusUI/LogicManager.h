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

  void UpdateThemeStatus();
  LRESULT MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  void PostMessageEventToAll(UINT uMsg, WPARAM wParam, LPARAM lParam);
  void PostMessageEventToAllEx(
      UINT                uMsg,
      WPARAM              wParam,
      LPARAM              lParam,
      PostMessageCallback proc
  );
  void PostMessageEventToTarget(
      Control* target,
      UINT     uMsg,
      WPARAM   wParam,
      LPARAM   lParam,
      bool     enableBubble = false
  );

  private:
  void _PostMessageEventToAll(
      Control* parent,
      UINT     uMsg,
      WPARAM   wParam,
      LPARAM   lParam
  );
  void _PostMessageEventToAllEx(
      Control*            parent,
      UINT                uMsg,
      WPARAM              wParam,
      LPARAM              lParam,
      PostMessageCallback proc
  );
  void _PostMessageEventToTarget(
      Control* target,
      UINT     uMsg,
      WPARAM   wParam,
      LPARAM   lParam,
      bool     enableBubble
  );

  private:
  HWND           m_hMessageWnd;
  RenderManager* m_renderManager;
};
} // namespace GdiplusUI

#endif // !_GDIPLUS_LOGICMANAGER_H_

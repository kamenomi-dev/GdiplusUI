// DemoWindow.cpp : 定义应用程序的入口点。
//

#include "DemoWindow.h"
#include "../GdiplusUI/GdiplusUI/Generic.h"

#pragma comment(lib, "GdiplusUI.lib")


HINSTANCE hInst;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  InitializeGdiplusUI();

  MyRegisterClass(hInstance);

  // 执行应用程序初始化:
  if (!InitInstance(hInstance, nCmdShow)) {
    return FALSE;
  }

  MSG msg;

  // 主消息循环:
  
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  UnInitializeGdiplusUI();

  return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
  WNDCLASSEXW wcex{};

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszClassName = L"DebugWindow";

  return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
  hInst = hInstance;  // 将实例句柄存储在全局变量中

  HWND hWnd = CreateWindowW(L"DebugWindow", L"DebugWindow", WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr,
                            nullptr, hInstance, nullptr);

  if (!hWnd) {
    return FALSE;
  }

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
                         LPARAM lParam) {
  static GdiplusUI::Components::Screen win{};
  static GdiplusUI::RenderManager renderManager(&win, hWnd);
  static GdiplusUI::LogicManager logicManager(&renderManager);

  if (message == WM_DESTROY) {
    PostQuitMessage(0);
    return 0;
  }

  return logicManager.MessageHandler(hWnd, message, wParam, lParam);
}

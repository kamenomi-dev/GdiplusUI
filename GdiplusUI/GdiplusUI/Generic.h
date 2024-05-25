#pragma once


#include <Windows.h>
#include <dwmapi.h>
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "Dwmapi.lib")

using namespace Gdiplus;


extern bool InitializeGdiplusUI();
extern bool UnInitializeGdiplusUI();


#include "./gdiplusUI.h"
#pragma once


#include <Windows.h>
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")

using namespace Gdiplus;



extern bool InitializeGdiplusUI();
extern bool UnInitializeGdiplusUI();


#include "./gdiplusUI.h"
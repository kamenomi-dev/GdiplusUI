
// #pragma warning(disable : 4251)

#include <assert.h>
#include <string>
#include <type_traits>
#include <vector>

#include <Windows.h>
#include <Windowsx.h>
#include <dwmapi.h>
#include <gdiplus.h>

using namespace Gdiplus;
using std::vector;
using std::wstring;

#ifdef GDIPLUSUI_EXPORTS
#define GpUI_API // _declspec(dllexport)
#else
#define GpUI_API // _declspec(dllimport)
#endif


#define _GDIPLUSUI_UTILS__DISABLE_ALL_WARNINGS
#define _GDIPLUSUI_UTILS__NO_CHANGE_ANY


#include "./Interface.h"

#include "./Utils/GdiplusUtils.h"
#include "./Utils/RegeditUtils.h"
#include "./Utils/WindowsUtils.h"

#include "./Components/Control.h"
#include "./Components/Frame.h"
#include "./Components/Screen.h"

#include "./RenderManager.h"

#include "./LogicManager.h"


// #pragma warning(disable : 4251)

#include <assert.h>
#include <filesystem>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include <Windows.h>
#include <Windowsx.h>
#include <dwmapi.h>
#include <gdiplus.h>

using namespace Gdiplus;
using std::string;
using std::vector;
using std::wstring;
namespace filesystem = std::filesystem;

#ifdef GDIPLUSUI_EXPORTS
#define GpUI_API // _declspec(dllexport)
#else
#define GpUI_API // _declspec(dllimport)
#endif


#include "./Includes/PugiXML/pugixml.hpp"

#include "./UIConfig.h"

#include "./Interface.h"

#include "./Utils/GdiplusUtils.h"
// #include "./Utils/ProgramUtils.h"
#include "./Utils/RegeditUtils.h"
#include "./Utils/WindowsUtils.h"

#include "./Components/Control.h"

#include "./ResourceManager.h"

#include "./RenderManager.h"

#include "./LogicManager.h"

#include "./Components/Frame.h"
#include "./Components/Screen.h"

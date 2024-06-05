#pragma once

#ifndef _WINDOWSUTILS_H_
#define _WINDOWSUTILS_H_

namespace GdiplusUI {
namespace Utils {

namespace Windows {
namespace WindowLayer {


namespace Defines {


// - - GetOsVersion();

typedef struct {
  ULONG NtMajorVersion;
  ULONG NtMinorVersion;
  ULONG NtBuildNumber;
} OsVersion;

// - - SetBlurEffect();

typedef enum { WCA_ACCENT_POLICY = 19 } WINDOWCOMPOSITIONATTRIB;

typedef enum { None = -2, Aero = 1, Acrylic = 2, Mica = 3 } BlurTypes;

/// <summary>
/// Todo. 添加 GRADIENT 支持，现在项目用不着，以后提issue再说 (2024.05.22)
/// </summary>
typedef enum {
  ACCENT_DISABLED                 = 0,
  ACCENT_ENABLE_BLURBEHIND        = 3,
  ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
  ACCENT_ENABLE_HOSTBACKDROP      = 5,
} ACCENT_STATE;

typedef struct {
  ACCENT_STATE AccentState;
  DWORD        AccentFlags;
  DWORD        GradientColor;
  DWORD        AnimationId;
} ACCENT_POLICY;

typedef struct {
  WINDOWCOMPOSITIONATTRIB Attrib;
  void*                   pvData;
  UINT                    cbData;
} WINDOWCOMPOSITIONATTRIBDATA;


typedef BOOL(__stdcall SetWindowCompositionAttribute)(
    HWND                                  hwnd,
    IN const WINDOWCOMPOSITIONATTRIBDATA* pwcad
);


} // namespace Defines

using namespace Defines;

static inline void
SetWindowCompositionAttributeFn(SetWindowCompositionAttribute*&, HMODULE&);

OsVersion GetOsVersion();
bool      SetDarkMode(HWND targetWindow, bool toggle);
bool      SetBlurEffect(HWND targetWindow, BlurTypes type = None);

HICON GetWindowIcon(HWND targetWindow);


} // namespace WindowLayer
} // namespace Windows


} // namespace Utils
} // namespace GdiplusUI

#endif _WINDOWSUTILS_H_

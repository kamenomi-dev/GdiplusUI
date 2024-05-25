#include "../gdiplusUI.h"

#include <VersionHelpers.h>


using GdiplusUI::Utils::Windows::WindowLayer::OsVersion;
using GdiplusUI::Utils::Windows::WindowLayer::SetWindowCompositionAttribute;


void GdiplusUI::Utils::Windows::WindowLayer::SetWindowCompositionAttributeFn(
    SetWindowCompositionAttribute*& fnSet,
    HMODULE&                        hUser32
) {

  hUser32 = LoadLibraryA("User32.dll");
  if (hUser32 == NULL) {
    return;
  }

  auto fnSetAttribute = (SetWindowCompositionAttribute*)
      GetProcAddress(hUser32, "SetWindowCompositionAttribute");

  if (fnSetAttribute == NULL) {
    FreeLibrary(hUser32);
    hUser32 = NULL;
    return;
  }

  fnSet = fnSetAttribute;
}


OsVersion GdiplusUI::Utils::Windows::WindowLayer::GetOsVersion() {
  // it uses KUSER_SHARED_DATA struct
  // (https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-kuser_shared_data)
  // KUSER_SHARED_DATA is exist at 0x7ffe0000 in every process memory.

  auto      sharedDataBase = (BYTE*)0x7ffe0000;
  OsVersion retVersion{};

  retVersion.NtMajorVersion =
      *(ULONG*)(sharedDataBase + 0x26c); // major version offset
  retVersion.NtMinorVersion =
      *(ULONG*)(sharedDataBase + 0x270); // minor version offset
  retVersion.NtBuildNumber =
      *(ULONG*)(sharedDataBase + 0x260); // build number offset

  return retVersion;
}


bool GdiplusUI::Utils::Windows::WindowLayer::SetDarkMode(HWND targetWindow, bool toggle) {
  BOOL  bValue      = toggle;                         
  
  return DwmSetWindowAttribute(
      targetWindow,
      DWMWA_USE_IMMERSIVE_DARK_MODE,
      &bValue,
      sizeof(bValue)
  ) == S_OK;
}


/// <summary>
/// Todo, 添加对 Mica 云母效果支持
/// </summary>
bool GdiplusUI::Utils::Windows::WindowLayer::SetBlurEffect(
    HWND      targetWindow,
    BlurTypes type
) {

  const auto IsWindows11 = GetOsVersion().NtBuildNumber >= 22000;

#if not IsWindows11 && not defined(_GDIPLUSUI_UTILS__DISABLE_ALL_WARNINGS)

  ///
  /// For developer.
  /// Here, I am so glad that you can view there code.
  ///
  /// The acrylic effect will be draggy in Windows 10 by Winapi.
  /// (except for UWP Application)
  ///
  /// And Mica effect has ONLY support for Windows 11 version
  /// or higher.
  ///

  assert(
      true
      && "Error! Acrylic effect and Mica effect haven't support lower than "
         "the Windows 11 version."
  );

#endif


  SetWindowCompositionAttribute* fn      = nullptr;
  HMODULE                        hUser32 = NULL;
  SetWindowCompositionAttributeFn(fn, hUser32);


  if (fn == NULL) {

    if (type != Aero) return false;

    if (IsWindows11) return false;

    DWM_BLURBEHIND blurBehind{
        DWM_BB_ENABLE | DWM_BB_BLURREGION, // dwFlags
        true,                              // fEnable
        NULL,                              // hRgnBlur
        false                              // fTransitionOnMaximized
    };

    return DwmEnableBlurBehindWindow(targetWindow, &blurBehind) == S_OK;
  }


  if (hUser32 == NULL) {
    return false;
  }


  Defines::ACCENT_POLICY accent{};
  accent.AccentState =
      (ACCENT_STATE)(type + 2); // ACCENT_ENABLE_BLURBEHIND - type.Aero = 2
  accent.AccentFlags   = 0;
  accent.AnimationId   = 0;
  accent.GradientColor = 0;


  Defines::WINDOWCOMPOSITIONATTRIBDATA attribData{};
  attribData.Attrib = WCA_ACCENT_POLICY;
  attribData.pvData = &accent;
  attribData.cbData = sizeof(accent);


  bool ret = false;
  if (fn != NULL) {
    ret = fn(targetWindow, &attribData);
  };


  FreeLibrary(hUser32);
  return ret;
}

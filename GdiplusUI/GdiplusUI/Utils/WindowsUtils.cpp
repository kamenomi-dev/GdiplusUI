#include "../gdiplusUI.h"

#include <VersionHelpers.h>

using GdiplusUI::Utils::Windows::WindowLayer::OsVersion;

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

/// <summary>
/// Todo, 添加对 Mica 云母效果支持
/// </summary>
bool GdiplusUI::Utils::Windows::WindowLayer::EnableBlurEffect(
    HWND      targetWindow,
    BlurTypes type
) {

  const auto IsWindows11 = GetOsVersion().NtBuildNumber >= 22000;

#if defined(_DEBUG) && not defined(_GDIPLUSUI_UTILS__DISABLE_ALL_WARNINGS)

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

  if (type >= BlurTypes::Acrylic) {

    assert(
        not IsWindows11
        && "Error! Acrylic effect and Mica effect haven't support lower than "
           "the Windows 11 version."
    );
  }

#endif // defined(_DEBUG) && not defined(_GDIPLUSUI_UTILS__DISABLE_ALL_WARNINGS)

#if not defined(_GDIPLUSUI_UTILS__NO_CHANGE_ANY)

  if (not IsWindows11) {
    if (type >= BlurTypes::Acrylic) {
      type = Aero;
    }
  }

#endif // defined(_GDIPLUSUI_UTILS__NO_CHANGE_ANY)

  auto hUser32 = LoadLibraryA("User32.dll");
  if (hUser32 == NULL) {
    return false;
  }

  auto fnSetAttribute = (SetWindowCompositionAttribute*)
      GetProcAddress(hUser32, "SetWindowCompositionAttribute");

  if (fnSetAttribute == nullptr) {
    FreeLibrary(hUser32);
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
  auto ret          = fnSetAttribute(targetWindow, &attribData);

  FreeLibrary(hUser32);
  return ret;
}

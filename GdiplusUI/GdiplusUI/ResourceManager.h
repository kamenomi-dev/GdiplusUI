#pragma once

#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

namespace GdiplusUI {

namespace Defines {

struct GeneralConf {
  Font*       DefaultFont;
  SolidBrush* DefaultFontBrush;

  Font*       ControlFont;
  SolidBrush* ControlFontBrush;
};

struct FrameConf {
  bool        UseRounded;
  Pen*        FrameColor;
  SolidBrush* BackgroundColor;
};

struct ColorTableConf {
  SolidBrush* NormalStatus;
  SolidBrush* HoverStatus;
  SolidBrush* DownStatus;
};

struct SymbolConf {
  std::unique_ptr<wstring> Minimize;
  std::unique_ptr<wstring> Maximize;
  std::unique_ptr<wstring> Close;
};

struct CaptionConf {
  ColorTableConf ColorTable;
  SymbolConf     Symbol;
};

struct WindowConf {
  string      UseEffect;
  FrameConf   Frame;
  CaptionConf Caption;
};

struct ThemeConf {
  GeneralConf General;
  WindowConf  Window;
};

} // namespace Defines


using namespace Defines;

class ResourceManager : public INonCopy {
  public:
  ResourceManager();
  ResourceManager(HWND hWnd, filesystem::path theme_file);
  ~ResourceManager();

  static ResourceManager* GetInstance();

  static ThemeConf& GetThemeConfig();

  static bool IsFrameRounded();

  static SymbolConf& GetControlSymbol();

  static SolidBrush& GetBackgroundBrush();
  static Pen&        GetBackgroundFramePen();

  static Font&       GetDefaultFont();
  static Font&       GetControlFont();
  static SolidBrush& GetDefaultFontBrush();
  static SolidBrush& GetControlFontBrush();
  static SolidBrush& GetCaptionButtonNormalBrush();
  static SolidBrush& GetCaptionButtonHoverBrush();
  static SolidBrush& GetCaptionButtonDownBrush();

  static Bitmap& GetWindowIcon();

  private:
  static ResourceManager* m_instance;

  wstring m_loadedTheme;

  ThemeConf m_themeConfig;

  HWND    m_hTargetWindow;
  Bitmap* m_wndIcon;
};

} // namespace GdiplusUI

#endif // _RESOURCEMANAGER_H_

#include "./gdiplusUI.h"


constexpr auto DEFAULT_THEME = L"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                               L"<ThemeConfig>"
                               L"	<General>"
                               L"		<DefaultFont font-size=\"16px\">Segoe UI</DefaultFont>"
                               L"		<ControlFont font-size=\"7px\">Segoe MDL2 Assets</ControlFont>"
                               L"   <DefaultFontColor>#FF000000</DefaultFontColor>"
                               L"   <ControlFontColor>#FF000000</ControlFontColor>"
                               L"	</General>"
                               L"	<Window>"
                               L"	    <UseEffect>Acrylic</UseEffect>"
                               L"		<Frame>"
                               L"			<UseRounded>true</UseRounded>"
                               L"			<FrameColor>#FF86837E</FrameColor>"
                               L"			<BackgroundColor>#FFF3F3F3</BackgroundColor>"
                               L"		</Frame>"
                               L"		<Caption>"
                               L"		    <ColorTable>"
                               L"		        <NormalStatus>#00000000</NormalStatus>"
                               L"		        <HoverStatus>#FF808080</HoverStatus>"
                               L"		        <DownStatus>#FFA9A9A9</DownStatus>"
                               L"		    </ColorTable>"
                               L"		    <Symbol>"
                               L"		       <Minimize>\uE921</Minimize>"
                               L"		       <Maximize>\uE922</Maximize>"
                               L"		       <Close>\uE8BB</Close>"
                               L"		    </Symbol>"
                               L"		</Caption>"
                               L"	</Window>"
                               L"</ThemeConfig>\0";


using std::make_unique;
using std::unique_ptr;
using namespace pugi;

using GdiplusUI::Utils::GdiplusExt::HexToColor;


GdiplusUI::ResourceManager* GdiplusUI::ResourceManager::m_instance = nullptr;


GdiplusUI::ResourceManager::ResourceManager() : ResourceManager(NULL, filesystem::path()) {}


GdiplusUI::ResourceManager::ResourceManager(HWND hWnd, filesystem::path theme_file) {
  m_instance      = this;
  m_hTargetWindow = hWnd;

  // - - - - - - Load Theme - - - - - - 

  m_themeConfig = ThemeConf{};

  xml_document     config{};
  xml_parse_result isOk{};
  if (not theme_file.empty()) {
    isOk = config.load_file(theme_file.c_str(), pugi::format_default, encoding_wchar);
    // Incorrect theme file.
  }

  if (theme_file.empty() or not isOk) {
    isOk = config.load_string(DEFAULT_THEME, pugi::format_default);
    // Using default theme.
  }

  if (not isOk) {
    throw new std::runtime_error(isOk.description());
  }

  m_loadedTheme = theme_file.c_str();



  const auto ThemeConfig = config.child(L"ThemeConfig");

  const auto General       = ThemeConfig.child(L"General");
  auto&      generalConfig = m_themeConfig.General;

  auto fontConfig                = General.child(L"ControlFont");
  generalConfig.ControlFont      = new Font(fontConfig.text().as_string(L"Segoe MDL2 Assets"), fontConfig.attribute(L"font-size").as_float(16));
  generalConfig.ControlFontBrush = new SolidBrush(HexToColor(General.child(L"ControlFontColor").text().as_string(L"#FFFFFFFF")));

  fontConfig                     = General.child(L"DefaultFont");
  generalConfig.DefaultFont      = new Font(fontConfig.text().as_string(L"Segoe UI"), fontConfig.attribute(L"font-size").as_float(7));
  generalConfig.DefaultFontBrush = new SolidBrush(HexToColor(General.child(L"DefaultFontColor").text().as_string(L"#FFFFFFFF")));



  const auto Window = ThemeConfig.child(L"Window");

  const auto Frame       = Window.child(L"Frame");
  auto&      frameConfig = m_themeConfig.Window.Frame;

  frameConfig.UseRounded      = Frame.child(L"UseRounded").text().as_bool(true);
  frameConfig.FrameColor      = new Pen(HexToColor(Frame.child(L"FrameColor").text().as_string(L"#FF86837E")), 1.5f);
  frameConfig.BackgroundColor = new SolidBrush(HexToColor(Frame.child(L"BackgroundColor").text().as_string(L"#FFF3F3F3")));



  const auto Caption          = Window.child(L"Caption");
  const auto ColorTable       = Caption.child(L"ColorTable");
  auto&      colorTableConfig = m_themeConfig.Window.Caption.ColorTable;

  colorTableConfig.NormalStatus = new SolidBrush(HexToColor(ColorTable.child(L"NormalStatus").text().as_string(L"#00000000")));
  colorTableConfig.HoverStatus  = new SolidBrush(HexToColor(ColorTable.child(L"HoverStatus").text().as_string(L"#FF808080")));
  colorTableConfig.DownStatus   = new SolidBrush(HexToColor(ColorTable.child(L"DownStatus").text().as_string(L"#FFA9A9A9")));


  const auto Symbol       = Caption.child(L"Symbol");
  auto&      symbolConfig = m_themeConfig.Window.Caption.Symbol;

  symbolConfig.Minimize = make_unique<wstring>(Symbol.child(L"Minimize").text().as_string(L"\uE921"));
  symbolConfig.Maximize = make_unique<wstring>(Symbol.child(L"Maximize").text().as_string(L"\uE922"));
  symbolConfig.Close    = make_unique<wstring>(Symbol.child(L"Close").text().as_string(L"\uE8BB"));

  // - - - - - - Load Window Interface - - - - - -

  m_wndIcon = new Bitmap(Utils::Windows::WindowLayer::GetWindowIcon(hWnd));
}

GdiplusUI::ResourceManager::~ResourceManager() {
}


GdiplusUI::ResourceManager* GdiplusUI::ResourceManager::GetInstance() { return ResourceManager::m_instance; }

GdiplusUI::ThemeConf& GdiplusUI::ResourceManager::GetThemeConfig() { return GetInstance()->m_themeConfig; }

bool GdiplusUI::ResourceManager::IsFrameRounded() { return GetThemeConfig().Window.Frame.UseRounded; }

GdiplusUI::SymbolConf& GdiplusUI::ResourceManager::GetControlSymbol() { return GetThemeConfig().Window.Caption.Symbol; }

SolidBrush& GdiplusUI::ResourceManager::GetBackgroundBrush() { return *GetThemeConfig().Window.Frame.BackgroundColor; }

Pen& GdiplusUI::ResourceManager::GetBackgroundFramePen() { return *GetThemeConfig().Window.Frame.FrameColor; }

Font& GdiplusUI::ResourceManager::GetDefaultFont() { return *GetThemeConfig().General.DefaultFont; }

Font& GdiplusUI::ResourceManager::GetControlFont() { return *GetThemeConfig().General.ControlFont; }

SolidBrush& GdiplusUI::ResourceManager::GetDefaultFontBrush() { return *GetThemeConfig().General.DefaultFontBrush; }

SolidBrush& GdiplusUI::ResourceManager::GetControlFontBrush() { return *GetThemeConfig().General.ControlFontBrush; }

SolidBrush& GdiplusUI::ResourceManager::GetCaptionButtonNormalBrush() { return *GetThemeConfig().Window.Caption.ColorTable.NormalStatus; }

SolidBrush& GdiplusUI::ResourceManager::GetCaptionButtonHoverBrush() { return *GetThemeConfig().Window.Caption.ColorTable.HoverStatus; }

SolidBrush& GdiplusUI::ResourceManager::GetCaptionButtonDownBrush() { return *GetThemeConfig().Window.Caption.ColorTable.DownStatus; }

Bitmap& GdiplusUI::ResourceManager::GetWindowIcon() { return *GetInstance()->m_wndIcon; }
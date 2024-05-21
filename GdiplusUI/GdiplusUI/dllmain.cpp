#include "./Generic.h"

extern bool g_globalInit = false;


static ULONG_PTR gdipToken{};

extern bool InitializeGdiplusUI() {
  GdiplusStartupInput input{};
  GdiplusStartup(&gdipToken, &input, NULL);
  return true;
}

extern bool UnInitializeGdiplusUI() {
  GdiplusShutdown(gdipToken);
  gdipToken = NULL;
  return true;
}

/*
static bool DllEntry(HMODULE hModule) {
  DisableThreadLibraryCalls(hModule);

  GdiplusStartupInput input{};
  GdiplusStartup(&gdipToken, &input, NULL);
  return true;
}

static bool DllExit() {
  GdiplusShutdown(gdipToken);
  gdipToken = NULL;
  return true;
}


BOOL APIENTRY
DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH: {
    g_globalInit = true;
    return DllEntry(hModule);
  };

  case DLL_PROCESS_DETACH: {
    return DllExit();
  };
  }

  return false;
}
*/
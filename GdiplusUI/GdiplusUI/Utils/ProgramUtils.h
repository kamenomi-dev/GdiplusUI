#pragma once
#ifndef _PROGRAMUTILS_H_
#define _PROGRAMUTILS_H_

namespace GdiplusUI {
namespace Utils {
namespace ProgramUtils {
class ProgramUtils : public INonCopy {
  public:
  ProgramUtils();
  ~ProgramUtils();

  static ProgramUtils* GetInstance();
  static HMODULE GetCurrentLibrary();

  HGLOBAL PickResource(unsigned long m_lResourceID, const wchar_t* m_strResourceType);
  void    FreeResourcePtr(HGLOBAL pRes);

  private:
  HMODULE m_hBase;

  static ProgramUtils* m_instance;
};
}; // namespace ProgramUtils
}; // namespace Utils
}; // namespace GdiplusUI

#endif // !_PROGRAMUTILS_H_

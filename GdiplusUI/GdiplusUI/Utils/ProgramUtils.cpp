#include "../gdiplusUI.h"

namespace ProgramUtils = GdiplusUI::Utils::ProgramUtils;

ProgramUtils::ProgramUtils* ProgramUtils::ProgramUtils::m_instance{};


ProgramUtils::ProgramUtils::ProgramUtils() : INonCopy() {
  GetModuleHandleExW(
      GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
      (LPCTSTR)ProgramUtils::ProgramUtils::GetCurrentLibrary,
      &m_hBase
  );

  m_instance = this;
}


ProgramUtils::ProgramUtils::~ProgramUtils() {
  FreeLibrary(m_hBase);
  m_hBase    = NULL;
  m_instance = nullptr;
}


ProgramUtils::ProgramUtils* GdiplusUI::Utils::ProgramUtils::ProgramUtils::GetInstance() { return m_instance; }


HMODULE ProgramUtils::ProgramUtils::GetCurrentLibrary() { return GetInstance()->m_hBase; }


void* ProgramUtils::ProgramUtils::PickResource(
    unsigned long  m_lResourceID,
    const wchar_t* m_strResourceType
) {
  HRSRC   hResID = FindResourceW(m_hBase, MAKEINTRESOURCEW(m_lResourceID), m_strResourceType);
  HGLOBAL hRes   = LoadResource(m_hBase, hResID);
  return hRes;
}

void ProgramUtils::ProgramUtils::FreeResourcePtr(HGLOBAL hRes) { FreeResource(hRes); }

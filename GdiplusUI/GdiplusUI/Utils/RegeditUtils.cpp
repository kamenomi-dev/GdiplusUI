#include "../gdiplusUI.h"

GdiplusUI::Utils::Regedit::Regedit::Regedit(wstring path) {
  m_path = path;
  m_hKey = NULL;

  HKEY          hRoot   = NULL;
  const wstring strRoot = path.substr(0, path.find(L"\\"));
  const wstring strPath = path.substr(path.find(L"\\") + 1);

  if (strRoot == L"HKEY_USERS") hRoot = HKEY_USERS;
  else if (strRoot == L"HKEY_LOCAL_MACHINE") hRoot = HKEY_LOCAL_MACHINE;
  else if (strRoot == L"HKEY_CURRENT_USER") hRoot = HKEY_CURRENT_USER;
  else if (strRoot == L"HKEY_CLASSES_ROOT") hRoot = HKEY_CLASSES_ROOT;
  else if (strRoot == L"HKEY_CURRENT_CONFIG") hRoot = HKEY_CURRENT_CONFIG;
  else return;

  auto result = RegOpenKeyExW(
      hRoot,
      strPath.c_str(),
      NULL,
      KEY_ALL_ACCESS | KEY_READ | KEY_WRITE,
      &m_hKey
  );

  if (result != ERROR_SUCCESS) {
    m_path = L"";
    m_hKey = NULL;
  }
}

GdiplusUI::Utils::Regedit::Regedit::~Regedit() {
  if (m_hKey != NULL) {
    RegCloseKey(m_hKey);
    m_hKey = NULL;
  }

  m_path = L"";
}

bool GdiplusUI::Utils::Regedit::Regedit::IsItemExist(wstring name) {
  assert(m_hKey);

  return RegQueryValueExW(m_hKey, name.c_str(), NULL, NULL, NULL, NULL)
      != ERROR_FILE_NOT_FOUND;
}

bool GdiplusUI::Utils::Regedit::Regedit::DeleteItem(wstring name) {
  assert(m_hKey);
  return RegDeleteKeyW(m_hKey, name.c_str()) == ERROR_SUCCESS;
}

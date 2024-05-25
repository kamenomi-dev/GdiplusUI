#pragma once

#ifndef _REGEDITUTILS_H_
#define _REGEDITUTILS_H_

#include <regex>

namespace GdiplusUI {
namespace Utils {
namespace Regedit {


class Regedit : GdiplusUI::INonCopy {
  public:
  Regedit(wstring path);
  ~Regedit();

  bool IsItemExist(wstring name);

  template <typename T>
  T GetItem(wstring name, T* defaultData = nullptr) {
    assert(m_hKey);

    ULONG flag = RRF_RT_REG_NONE;
    // if (std::is_same_v<T, DWORD32>) flag = RRF_RT_DWORD | RRF_RT_REG_DWORD;
    // if (std::is_same_v<T, DWORD64>) flag = RRF_RT_QWORD | RRF_RT_REG_QWORD;
    if (std::is_same_v<T, void>) flag = RRF_RT_REG_NONE;
    else if (std::is_same_v<T, byte>) flag = RRF_RT_REG_BINARY;
    else if (std::is_same_v<T, DWORD>) flag = RRF_RT_REG_DWORD;
    else if (std::is_same_v<T, DWORD64>) flag = RRF_RT_REG_QWORD;
    // else if (std::is_same_v<T, wchar_t*>) flag = RRF_RT_REG_EXPAND_SZ;
    // else if (std::is_same_v<T, wchar_t[]>) flag = RRF_RT_REG_MULTI_SZ;

    void* data{};
    DWORD dataLength{};
    RegGetValueW(
        m_hKey,
        L"",
        name.c_str(),
        RRF_ZEROONFAILURE | flag,
        NULL,
        NULL,
        &dataLength
    );

    data = malloc(dataLength);
    auto result = RegGetValueW(
        m_hKey,
        L"",
        name.c_str(),
        RRF_ZEROONFAILURE | flag,
        NULL,
        data,
        &dataLength
    );

    T ret = T(*(T*)(data));
    free(data);

    if (result != ERROR_SUCCESS) {
      return *defaultData;
    }
    return ret;
  }

  template <typename T>
  T SetItem(wstring name, T data) {
    assert(m_hKey);

    auto lastData = GetItem<T>(name);
    auto dataType = REG_NONE;

    if (std::is_same_v<T, char>) dataType = REG_BINARY;
    else if (std::is_same_v<T, DWORD>) dataType = REG_DWORD;
    else if (std::is_same_v<T, DWORD64>) dataType = REG_QWORD;
    else if (std::is_same_v<T, char>) dataType = REG_SZ;
    else if (std::is_same_v<T, wchar_t>) dataType = REG_EXPAND_SZ;

    RegSetValueExW(m_hKey, name, NULL, dataType, &data, sizeof(data));

    return lastData;
  }

  bool DeleteItem(wstring name);

  private:
  HKEY    m_hKey;
  wstring m_path;
};


} // namespace Regedit
} // namespace Utils
} // namespace GdiplusUI

#endif // !_REGEDITUTILS_H_

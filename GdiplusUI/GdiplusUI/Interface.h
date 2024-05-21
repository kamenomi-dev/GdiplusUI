#pragma once


namespace GdiplusUI {
class GpUI_API INonCopy abstract {
  public:
  INonCopy()  = default;
  ~INonCopy() = default;

  private:
  INonCopy(const INonCopy&)            = delete;
  INonCopy& operator=(const INonCopy&) = delete;
};
} // namespace GdiplusUI
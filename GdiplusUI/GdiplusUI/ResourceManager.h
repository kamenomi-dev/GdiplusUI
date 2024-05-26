#pragma once

#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

namespace GdiplusUI {

class ResourceManager : public INonCopy {
  public:
  ResourceManager();
  ~ResourceManager();

  Font& GetControlFont();

  private:
  Font* m_controlFont;
};

} // namespace GdiplusUI
#endif // _RESOURCEMANAGER_H_

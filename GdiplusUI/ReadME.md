# README

>[!NOTE]
> 现在你所处的分支位置为 **旧的项目仓库**。这只是做一个存档，旧的项目仅支持单个窗口的绘制，并不支持多自定义窗口样式。

---
作者的话：

本项目在 Windows 2H22，VisualStudio 2022，C++20 的环境下编译运行。
本项目基于 Gdiplus库，即底层Gdi的扩展库进行开发，应有长期支持。
本项目是基于微软给予 Windows 长兼容性开发，可能遇到内存错误等问题，可以通过绕弯子的办法解决。

**这个项目就是一个屎山，有点参考意义（因为这个仓库浓缩了来自中文互联网对 Windows API的探究）**

原本之前想用动态库实现 UI 库的，但是编译期过不了，无奈转为静态库了~

---

> [!NOTE]
> 有 **任何** 编译或运行不正常的原因可以先尝试自己修复，再提交 PullRequest 或 Issues，因为有许多问题并不能在一个系统上复现。

> [!WARNING]
> **GdiplusUI 库** 将所有类都定义了 **缺省构造函数** ，但是不建议这样构造，因为他会导致非预期错误。

## 库运行方式

```C++
// 调用方窗口消息函数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
                         LPARAM lParam) {
  static GdiplusUI::Components::Screen win{};
  static GdiplusUI::RenderManager renderManager(&win, hWnd);
  static GdiplusUI::LogicManager logicManager(&renderManager);

  if (message == WM_DESTROY) {
    PostQuitMessage(0);
    return 0;
  }

  return logicManager.MessageHandler(hWnd, message, wParam, lParam);
}
```

第一次进入 `WndProc` 函数时初始化 Screen、renderManager 等类。  
先后顺序为 组件创建，渲染管理器，逻辑管理器：  
1. 组件创建完并修改好子父关系，确保 Screen 为 最顶层组件否则会产生非预期错误。
2. Screen引用 以及 窗口句柄 传递至 RenderManager 的初始化函数，所有有关渲染都在此处理。  
3. RenderManager引用 传递至 logicManager 的初始化函数，所有逻辑都在此处理。

在函数末尾返回时，直接返回 logicManager.MessageHandler函数结果，它只会返回它需要的消息处理结果。

## 消息传递

> 对于任意组件，在 Wndproc传递渲染消息时都从 根组件(Screen) 开始自上而下开始；  
> 对于任意组件，在 Wndproc传递逻辑消息时都从 目标组件 开始自下而上开始（冒泡）；

### WM_PAINT

处理 WM_PAINT 时，不采用 Windows 约定方式，而是创建一个渲染Graphics来传递消息给组件。

```C++
    // ... (LogicManager.cpp)

if (uMsg == WM_PAINT) {
    assert(swapChain);

    Graphics presentGrap(swapChain->GetLayoutDC());
    auto     ret = presentGrap.GetLastStatus();
    presentGrap.Clear(Color(themeColor)); // Todo.

    PostMessageEventToAllEx(
        WM_PAINT,
        NULL,
        (LPARAM)&presentGrap,
        [](PostMsgCbDataStruct data) -> void {
          if (data.lastTarget == nullptr) {
            return;
          }

          auto lastControl = data.lastTarget;
          auto presentGrap = RenderManager::GetGraphics(data.lParam);
          presentGrap->ExcludeClip(lastControl->GetRect());
        }
    );

    swapChain->Present();
  }

  // ...
```

这代码块或许难看（你也可以提交 PullRequest 优化），但是将传递消息给组件的方式通用化，显得没那么冗余。

>这代码块有个不足的点就是子组件没有Clip的限制会渲染到父组件以外的地方，埋个坑。  
>但是 ExcludeClip 好像又弥补了这个缺点，它会将已经绘制的地方排除掉，防止已绘制的地方覆盖，画出来的地方不算，这歪打正着了。

## WM_ 键鼠消息

> [!INFO]
> 这里为未来开发的方向，在实际代码中可能没有具体实现。

### WM_ 鼠标消息

> 这里引入一个以组件指针存储的 currentFocusCompoent 变量，记为 CFComp。

库初始化的时候，CFComp 会默认设置为Screen。  
在触发鼠标消息前，会经过 Screen 的命中测试函数，若不是客户区就拦截，反之传递给正在捕获鼠标消息的 CFComp 的组件，最后再传递后续程序。

在触发消息后，首先会调用 CFComp指向的组件告诉已经失去焦点，再经过 逻辑控制器 并调用 渲染控制器的 FindControl 函数，将结果赋值给 CFComp。
接着调用现行 CFComp 指向的组件告诉已经获得焦点。

> [!WARNING]
> WM_SET/KILLFOCUS 消息和所传递焦点消息并不一样，这为了维护Windows的消息约定，尽管传递焦点的消息值不来自于RegisterWindowMessage（没有人这么干吧？）

### WM_ 键盘消息

引用上篇 WM_ 鼠标消息定义: CFComp变量。

在接收鼠标消息时，逻辑管理器会判断 CFComp 是否为 Edit/RichEdit 组件等会捕获键盘消息的组件，然后传递给该组件，或者将消息传递贵给 根组件（Screen）。
在 Edit/RichEdit 等自带组件接收消息时，会结合 IME 输入法api 来进行优化输入。
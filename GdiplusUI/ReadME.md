# README

>[!NOTE]
> �����������ķ�֧λ��Ϊ **�ɵ���Ŀ�ֿ�**����ֻ����һ���浵���ɵ���Ŀ��֧�ֵ������ڵĻ��ƣ�����֧�ֶ��Զ��崰����ʽ��

---
���ߵĻ���

����Ŀ�� Windows 2H22��VisualStudio 2022��C++20 �Ļ����±������С�
����Ŀ���� Gdiplus�⣬���ײ�Gdi����չ����п�����Ӧ�г���֧�֡�
����Ŀ�ǻ���΢����� Windows �������Կ��������������ڴ��������⣬����ͨ�������ӵİ취�����

**�����Ŀ����һ��ʺɽ���е�ο����壨��Ϊ����ֿ�Ũ�����������Ļ������� Windows API��̽����**

ԭ��֮ǰ���ö�̬��ʵ�� UI ��ģ����Ǳ����ڹ����ˣ�����תΪ��̬����~

---

> [!NOTE]
> �� **�κ�** ��������в�������ԭ������ȳ����Լ��޸������ύ PullRequest �� Issues����Ϊ��������Ⲣ������һ��ϵͳ�ϸ��֡�

> [!WARNING]
> **GdiplusUI ��** �������඼������ **ȱʡ���캯��** �����ǲ������������죬��Ϊ���ᵼ�·�Ԥ�ڴ���

## �����з�ʽ

```C++
// ���÷�������Ϣ����
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

��һ�ν��� `WndProc` ����ʱ��ʼ�� Screen��renderManager ���ࡣ  
�Ⱥ�˳��Ϊ �����������Ⱦ���������߼���������  
1. ��������겢�޸ĺ��Ӹ���ϵ��ȷ�� Screen Ϊ ����������������Ԥ�ڴ���
2. Screen���� �Լ� ���ھ�� ������ RenderManager �ĳ�ʼ�������������й���Ⱦ���ڴ˴���  
3. RenderManager���� ������ logicManager �ĳ�ʼ�������������߼����ڴ˴���

�ں���ĩβ����ʱ��ֱ�ӷ��� logicManager.MessageHandler�����������ֻ�᷵������Ҫ����Ϣ��������

## ��Ϣ����

> ��������������� Wndproc������Ⱦ��Ϣʱ���� �����(Screen) ��ʼ���϶��¿�ʼ��  
> ��������������� Wndproc�����߼���Ϣʱ���� Ŀ����� ��ʼ���¶��Ͽ�ʼ��ð�ݣ���

### WM_PAINT

���� WM_PAINT ʱ�������� Windows Լ����ʽ�����Ǵ���һ����ȾGraphics��������Ϣ�������

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

����������ѿ�����Ҳ�����ύ PullRequest �Ż��������ǽ�������Ϣ������ķ�ʽͨ�û����Ե�û��ô���ࡣ

>�������и�����ĵ���������û��Clip�����ƻ���Ⱦ�����������ĵط�������ӡ�  
>���� ExcludeClip �������ֲ������ȱ�㣬���Ὣ�Ѿ����Ƶĵط��ų�������ֹ�ѻ��Ƶĵط����ǣ��������ĵط����㣬����������ˡ�

## WM_ ������Ϣ

> [!INFO]
> ����Ϊδ�������ķ�����ʵ�ʴ����п���û�о���ʵ�֡�

### WM_ �����Ϣ

> ��������һ�������ָ��洢�� currentFocusCompoent ��������Ϊ CFComp��

���ʼ����ʱ��CFComp ��Ĭ������ΪScreen��  
�ڴ��������Ϣǰ���ᾭ�� Screen �����в��Ժ����������ǿͻ��������أ���֮���ݸ����ڲ��������Ϣ�� CFComp �����������ٴ��ݺ�������

�ڴ�����Ϣ�����Ȼ���� CFCompָ�����������Ѿ�ʧȥ���㣬�پ��� �߼������� ������ ��Ⱦ�������� FindControl �������������ֵ�� CFComp��
���ŵ������� CFComp ָ�����������Ѿ���ý��㡣

> [!WARNING]
> WM_SET/KILLFOCUS ��Ϣ�������ݽ�����Ϣ����һ������Ϊ��ά��Windows����ϢԼ�������ܴ��ݽ������Ϣֵ��������RegisterWindowMessage��û������ô�ɰɣ���

### WM_ ������Ϣ

������ƪ WM_ �����Ϣ����: CFComp������

�ڽ��������Ϣʱ���߼����������ж� CFComp �Ƿ�Ϊ Edit/RichEdit ����ȻᲶ�������Ϣ�������Ȼ�󴫵ݸ�����������߽���Ϣ���ݹ�� �������Screen����
�� Edit/RichEdit ���Դ����������Ϣʱ������ IME ���뷨api �������Ż����롣
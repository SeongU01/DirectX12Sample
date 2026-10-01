#include "pch.h"
#include "FpsCameraController.h"
#include <iostream>

namespace
{
const HWND TestWindow = reinterpret_cast<HWND>(1);
HWND foreground = TestWindow;
HWND capture = nullptr;
POINT cursor{1000, 600};
bool keys[256]{};
bool clipped = false;
bool minimized = false;
int cursorVisibility = 0;
FpsCameraController* receiver = nullptr;

SHORT TestKeyState(int key) { return keys[key] ? static_cast<SHORT>(0x8000) : 0; }
HWND TestForeground() { return foreground; }
BOOL TestIconic(HWND) { return minimized; }
HWND TestCapture() { return capture; }
BOOL TestCursorPos(POINT* value) { *value = cursor; return TRUE; }
BOOL TestSetCursor(int x, int y) { cursor = {x, y}; return TRUE; }
BOOL TestClientRect(HWND, RECT* value) { *value = {0, 0, 1600, 900}; return TRUE; }
BOOL TestClientToScreen(HWND, POINT* value) { value->x += 100; value->y += 100; return TRUE; }
HWND TestSetCapture(HWND value) { const HWND old = capture; capture = value; return old; }
BOOL TestClip(const RECT* value) { clipped = value != nullptr; return TRUE; }
int TestShowCursor(BOOL show) { cursorVisibility += show ? 1 : -1; return cursorVisibility; }
BOOL TestRelease()
{
    capture = nullptr;
    receiver->ProcessWindowMessage(TestWindow, WM_CAPTURECHANGED, 0, 0);
    return TRUE;
}

void Check(bool value, const char* message)
{
    if (!value) throw std::runtime_error(message);
}
}

// 실제 Win32 어댑터를 그대로 컴파일하되 OS 경계만 대체해 사용자 입력/커서에는 영향 없이 검사한다.
#define GetAsyncKeyState TestKeyState
#define GetForegroundWindow TestForeground
#define IsIconic TestIconic
#define GetCapture TestCapture
#define GetCursorPos TestCursorPos
#define SetCursorPos TestSetCursor
#define GetClientRect TestClientRect
#define ClientToScreen TestClientToScreen
#define SetCapture TestSetCapture
#define ClipCursor TestClip
#define ShowCursor TestShowCursor
#define ReleaseCapture TestRelease
#include "../DirectX12Sample/FpsCameraControllerWin32.cpp"
#undef GetAsyncKeyState
#undef GetForegroundWindow
#undef IsIconic
#undef GetCapture
#undef GetCursorPos
#undef SetCursorPos
#undef GetClientRect
#undef ClientToScreen
#undef SetCapture
#undef ClipCursor
#undef ShowCursor
#undef ReleaseCapture

int main()
{
    try
    {
        Camera camera;
        FpsCameraController controller;
        receiver = &controller;
        controller.SetPose(camera, {}, {0.0f, 0.0f, 1.0f});
        const auto update = [&](UIInputCapture ui = {}) { controller.Update(TestWindow, camera, 0.1f, ui); };
        const auto released = [&] {
            Check(!controller.IsActive() && !clipped && cursorVisibility == 0, "capture and cursor released");
        };
        const auto activate = [&] {
            keys[VK_RBUTTON] = false;
            update();
            cursor = {1000, 600};
            keys[VK_RBUTTON] = true;
            update();
            Check(controller.IsActive() && clipped && capture == TestWindow && cursorVisibility < 0,
                  "right button activates capture");
        };

        keys['W'] = true;
        update();
        Check(controller.GetPosition().z == 0.0f, "no movement without right button");
        keys[VK_RBUTTON] = true;
        update({.mouse = true});
        released();
        update();
        released();
        Check(controller.GetPosition().z == 0.0f, "UI click cannot transfer ownership while held");
        activate();
        Check(controller.GetPosition().z == 0.25f, "W key mapped to movement");
        keys['W'] = false;
        cursor.x += 40;
        cursor.y += 20;
        update();
        Check(controller.GetPitch() < 0.0f && cursor.x == 900 && cursor.y == 550, "mouse delta and recenter");
        update({.keyboard = true});
        released();
        Check(cursor.x == 1000 && cursor.y == 600, "cursor restored after input block");

        activate();
        controller.ProcessWindowMessage(TestWindow, WM_RBUTTONUP, 0, 0);
        released();
        update();
        released();
        activate();
        keys[VK_ESCAPE] = true;
        update();
        released();
        keys[VK_ESCAPE] = false;
        activate();
        foreground = reinterpret_cast<HWND>(2);
        update();
        released();
        foreground = TestWindow;

        for (const UINT message : {WM_KILLFOCUS, WM_ACTIVATEAPP, WM_ENTERSIZEMOVE, WM_CAPTURECHANGED, WM_DESTROY})
        {
            activate();
            controller.ProcessWindowMessage(TestWindow, message, 0, 0);
            released();
        }
        activate();
        controller.ProcessWindowMessage(TestWindow, WM_SIZE, SIZE_MINIMIZED, 0);
        released();
        activate();
        controller.ReleaseInput();
        released();
        Check(capture == nullptr, "shutdown releases Win32 capture");
        std::cout << "FPS input contract: PASS\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

#include "pch.h"
#include "FpsCameraController.h"

namespace
{
bool KeyDown(int key)
{
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}
}

FpsCameraController::~FpsCameraController()
{
    ReleaseInput();
}

void FpsCameraController::Update(HWND window, Camera& camera, float deltaTime, const UIInputCapture& uiCapture)
{
    const bool rightDown = KeyDown(VK_RBUTTON);
    const bool pressed = rightDown && !_rightWasDown;
    _rightWasDown = rightDown;
    if (GetForegroundWindow() != window || IsIconic(window) || !rightDown || KeyDown(VK_ESCAPE) ||
        uiCapture.keyboard)
    {
        EndMouseCapture(GetForegroundWindow() == window);
        return;
    }

    // UI에서 시작한 우클릭은 누른 채 뷰포트로 이동해도 카메라 입력으로 바뀌지 않는다.
    if (!IsActive())
    {
        if (!pressed || uiCapture.mouse || !BeginMouseCapture(window))
        {
            return;
        }
    }
    if (GetCapture() != window)
    {
        EndMouseCapture(false);
        return;
    }

    POINT cursor{};
    if (!GetCursorPos(&cursor))
    {
        EndMouseCapture(false);
        return;
    }
    Input input;
    input.lookDelta = {static_cast<float>(cursor.x - _mouseAnchor.x),
                       static_cast<float>(cursor.y - _mouseAnchor.y)};
    input.movement = {static_cast<float>(KeyDown('D')) - static_cast<float>(KeyDown('A')),
                      static_cast<float>(KeyDown('E')) - static_cast<float>(KeyDown('Q')),
                      static_cast<float>(KeyDown('W')) - static_cast<float>(KeyDown('S'))};
    input.boost = KeyDown(VK_SHIFT);
    SetCursorPos(_mouseAnchor.x, _mouseAnchor.y);
    Update(camera, input, deltaTime);
}

bool FpsCameraController::BeginMouseCapture(HWND window)
{
    RECT bounds{};
    POINT origin{};
    POINT cursor{};
    if (!GetClientRect(window, &bounds) || !ClientToScreen(window, &origin) || !GetCursorPos(&cursor))
    {
        return false;
    }
    OffsetRect(&bounds, origin.x, origin.y);
    if (!PtInRect(&bounds, cursor) || (GetCapture() && GetCapture() != window))
    {
        return false;
    }
    SetCapture(window);
    if (GetCapture() != window)
    {
        return false;
    }
    _capturedWindow = window;
    _restoreCursor = cursor;
    _mouseAnchor = {(bounds.left + bounds.right) / 2, (bounds.top + bounds.bottom) / 2};
    if (!ClipCursor(&bounds) || !SetCursorPos(_mouseAnchor.x, _mouseAnchor.y))
    {
        EndMouseCapture(true);
        return false;
    }
    int visibility;
    do
    {
        visibility = ShowCursor(FALSE);
        ++_cursorHideCalls;
    } while (visibility >= 0);
    return true;
}

void FpsCameraController::EndMouseCapture(bool restorePosition)
{
    if (!IsActive())
    {
        return;
    }
    // ReleaseCapture가 재진입 메시지를 발생시키므로 소유 상태부터 해제한다.
    const HWND window = _capturedWindow;
    _capturedWindow = nullptr;
    ClipCursor(nullptr);
    while (_cursorHideCalls > 0)
    {
        ShowCursor(TRUE);
        --_cursorHideCalls;
    }
    if (GetCapture() == window)
    {
        ReleaseCapture();
    }
    if (restorePosition && GetForegroundWindow() == window)
    {
        SetCursorPos(_restoreCursor.x, _restoreCursor.y);
    }
}

void FpsCameraController::ReleaseInput()
{
    EndMouseCapture(false);
}

void FpsCameraController::ProcessWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (window != _capturedWindow)
    {
        return;
    }
    if (message == WM_RBUTTONUP)
    {
        EndMouseCapture(true);
    }
    else if (message == WM_KILLFOCUS || message == WM_DESTROY || message == WM_ENTERSIZEMOVE ||
             (message == WM_ACTIVATEAPP && !wParam) || (message == WM_SIZE && wParam == SIZE_MINIMIZED) ||
             (message == WM_CAPTURECHANGED && reinterpret_cast<HWND>(lParam) != window))
    {
        EndMouseCapture(false);
    }
}

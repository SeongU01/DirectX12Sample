#pragma once
#include "pch.h"
#include "Camera.h"

class FpsCameraController
{
public:
    struct Settings
    {
        float moveSpeed = 2.5f;
        float boostMultiplier = 3.0f;
        float mouseSensitivity = 0.0025f;
    };

    struct Input
    {
        XMFLOAT3 movement{}; // X: 오른쪽, Y: 위쪽, Z: 전방 입력.
        XMFLOAT2 lookDelta{};
        bool boost = false;
    };

    FpsCameraController() = default;
    ~FpsCameraController();
    FpsCameraController(const FpsCameraController&) = delete;
    FpsCameraController& operator=(const FpsCameraController&) = delete;

    void SetSettings(const Settings& settings);
    void SetPose(Camera& camera, const XMFLOAT3& position, const XMFLOAT3& target);
    // 입력 장치와 분리된 이동 계산은 다른 플랫폼/입력 공급자에서도 재사용한다.
    void Update(Camera& camera, const Input& input, float deltaTime);
    void Update(HWND window, Camera& camera, float deltaTime, const UIInputCapture& uiCapture);
    void ProcessWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
    void ReleaseInput();

    const XMFLOAT3& GetPosition() const { return _position; }
    float GetPitch() const { return _pitch; }
    bool IsActive() const { return _capturedWindow != nullptr; }

private:
    void ApplyView(Camera& camera) const;
    bool BeginMouseCapture(HWND window);
    void EndMouseCapture(bool restorePosition);

    Settings _settings;
    XMFLOAT3 _position{};
    float _yaw = 0.0f;
    float _pitch = 0.0f;
    HWND _capturedWindow = nullptr;
    POINT _restoreCursor{};
    POINT _mouseAnchor{};
    int _cursorHideCalls = 0;
    bool _rightWasDown = false;
};

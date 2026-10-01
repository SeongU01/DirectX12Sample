#include "pch.h"
#include "FpsCameraController.h"
#include <cmath>

namespace
{
constexpr float PitchLimit = XM_PIDIV2 - 0.01f;
}

void FpsCameraController::SetSettings(const Settings& settings)
{
    if (!std::isfinite(settings.moveSpeed) || settings.moveSpeed <= 0.0f ||
        !std::isfinite(settings.boostMultiplier) || settings.boostMultiplier < 1.0f ||
        !std::isfinite(settings.mouseSensitivity) || settings.mouseSensitivity <= 0.0f)
    {
        throw std::invalid_argument("FPS camera settings must be finite and positive");
    }
    _settings = settings;
}

void FpsCameraController::SetPose(Camera& camera, const XMFLOAT3& position, const XMFLOAT3& target)
{
    const XMVECTOR direction = XMLoadFloat3(&target) - XMLoadFloat3(&position);
    const float length = XMVectorGetX(XMVector3Length(direction));
    if (!std::isfinite(length) || length < 0.0001f)
    {
        throw std::invalid_argument("FPS camera position and target must define a direction");
    }
    _position = position;
    _yaw = std::atan2(XMVectorGetX(direction), XMVectorGetZ(direction));
    _pitch = std::clamp(std::asin(std::clamp(XMVectorGetY(direction) / length, -1.0f, 1.0f)),
                        -PitchLimit, PitchLimit);
    ApplyView(camera);
}

void FpsCameraController::Update(Camera& camera, const Input& input, float deltaTime)
{
    // 마우스 이동량에는 시간을 곱하지 않고, 이동 속도에만 초 단위 시간을 적용한다.
    _yaw = std::remainder(_yaw + input.lookDelta.x * _settings.mouseSensitivity, XM_2PI);
    _pitch = std::clamp(_pitch - input.lookDelta.y * _settings.mouseSensitivity, -PitchLimit, PitchLimit);
    const XMVECTOR forward = XMVectorSet(std::sin(_yaw), 0.0f, std::cos(_yaw), 0.0f);
    const XMVECTOR right = XMVectorSet(std::cos(_yaw), 0.0f, -std::sin(_yaw), 0.0f);
    XMVECTOR movement = forward * input.movement.z + right * input.movement.x +
                        XMVectorSet(0.0f, input.movement.y, 0.0f, 0.0f);
    if (XMVectorGetX(XMVector3LengthSq(movement)) > 1.0f)
    {
        movement = XMVector3Normalize(movement);
    }
    // 디버거 중단/창 전환 후 긴 프레임으로 카메라가 순간 이동하지 않도록 제한한다.
    const float step = std::isfinite(deltaTime) ? std::clamp(deltaTime, 0.0f, 0.1f) : 0.0f;
    const float speed = _settings.moveSpeed * (input.boost ? _settings.boostMultiplier : 1.0f);
    XMStoreFloat3(&_position, XMLoadFloat3(&_position) + movement * (speed * step));
    ApplyView(camera);
}

void FpsCameraController::ApplyView(Camera& camera) const
{
    const float horizontal = std::cos(_pitch);
    const XMFLOAT3 target{_position.x + std::sin(_yaw) * horizontal,
                         _position.y + std::sin(_pitch), _position.z + std::cos(_yaw) * horizontal};
    camera.SetLookAt(_position, target, {0.0f, 1.0f, 0.0f});
}

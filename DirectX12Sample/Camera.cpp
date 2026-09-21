#include "pch.h"
#include "Camera.h"

Camera::Camera()
{
    XMStoreFloat4x4(&_renderView.view, XMMatrixIdentity());
    XMStoreFloat4x4(&_renderView.projection, XMMatrixIdentity());
}

void Camera::SetLookAt(const XMFLOAT3& position, const XMFLOAT3& target, const XMFLOAT3& up)
{
    const XMVECTOR eyePosition = XMLoadFloat3(&position);
    const XMVECTOR focusPoint  = XMLoadFloat3(&target);
    const XMVECTOR upDirection = XMLoadFloat3(&up);

    XMStoreFloat4x4(&_renderView.view, XMMatrixLookAtLH(eyePosition, focusPoint, upDirection));
}

void Camera::SetPerspective(float verticalFov, float aspectRatio, float nearPlane, float farPlane)
{
    XMStoreFloat4x4(&_renderView.projection,
                    XMMatrixPerspectiveFovLH(verticalFov, aspectRatio, nearPlane, farPlane));
}

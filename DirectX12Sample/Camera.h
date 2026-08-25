#pragma once
#include "Graphics_Structs.h"

class Camera
{
public:
    Camera();

    void SetLookAt(const XMFLOAT3& position, const XMFLOAT3& target, const XMFLOAT3& up);
    void SetPerspective(float verticalFov, float aspectRatio, float nearPlane, float farPlane);

    const RenderView& GetRenderView() const { return _renderView; }

private:
    RenderView _renderView;
};

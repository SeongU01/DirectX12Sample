#include "pch.h"
#include "FpsCameraController.h"
#include <cmath>
#include <iostream>

namespace
{
void Check(bool value, const char* message)
{
    if (!value)
    {
        throw std::runtime_error(message);
    }
}

bool Near(float a, float b)
{
    return std::abs(a - b) < 0.0001f;
}

float Distance(const XMFLOAT3& value)
{
    return XMVectorGetX(XMVector3Length(XMLoadFloat3(&value)));
}
}

int main()
{
    try
    {
        Camera camera;
        camera.SetPerspective(XM_PIDIV4, 16.0f / 9.0f, 0.1f, 100.0f);
        const auto projection = camera.GetRenderView().projection;
        FpsCameraController controller;
        controller.SetPose(camera, {}, {0.0f, 0.0f, 1.0f});
        controller.Update(camera, {.movement = {0.0f, 0.0f, 1.0f}}, 0.1f);
        Check(Near(controller.GetPosition().z, 0.25f), "forward speed");
        Check(Near(camera.GetRenderView().view._43, -0.25f), "camera view updated");
        Check(Near(camera.GetRenderView().projection._11, projection._11), "projection preserved");
        controller.Update(camera, {.movement = {0.0f, 0.0f, -1.0f}}, 0.1f);
        Check(Near(Distance(controller.GetPosition()), 0.0f), "backward cancels forward");
        controller.Update(camera, {.movement = {1.0f, 0.0f, 1.0f}}, 0.1f);
        Check(Near(Distance(controller.GetPosition()), 0.25f), "diagonal speed normalized");

        controller.SetPose(camera, {}, {0.0f, 0.0f, 1.0f});
        controller.Update(camera, {.movement = {0.0f, 0.0f, 1.0f}}, 0.05f);
        controller.Update(camera, {.movement = {0.0f, 0.0f, 1.0f}}, 0.05f);
        Check(Near(controller.GetPosition().z, 0.25f), "frame-rate independent movement");
        controller.SetPose(camera, {}, {0.0f, 0.0f, 1.0f});
        controller.Update(camera, {.movement = {0.0f, 0.0f, 1.0f}, .boost = true}, 0.1f);
        Check(Near(controller.GetPosition().z, 0.75f), "shift speed multiplier");

        controller.SetPose(camera, {}, {0.0f, 1.0f, 1.0f});
        controller.Update(camera, {.movement = {0.0f, 0.0f, 1.0f}}, 0.1f);
        Check(Near(controller.GetPosition().y, 0.0f), "FPS forward stays horizontal");
        controller.Update(camera, {.movement = {0.0f, 1.0f, 0.0f}}, 0.1f);
        Check(Near(controller.GetPosition().y, 0.25f), "vertical movement");
        controller.Update(camera, {.movement = {0.0f, -1.0f, 0.0f}}, 0.1f);
        Check(Near(controller.GetPosition().y, 0.0f), "vertical reverse");

        controller.SetPose(camera, {}, {0.0f, 0.0f, 1.0f});
        controller.Update(camera, {.lookDelta = {XM_PIDIV2 / 0.0025f, 0.0f}}, 0.0f);
        controller.Update(camera, {.movement = {0.0f, 0.0f, 1.0f}}, 0.1f);
        Check(Near(controller.GetPosition().x, 0.25f) && Near(controller.GetPosition().z, 0.0f),
              "movement follows yaw");
        controller.Update(camera, {.lookDelta = {0.0f, -100000.0f}}, 0.1f);
        Check(controller.GetPitch() < XM_PIDIV2 && controller.GetPitch() > 1.5f, "upper pitch limit");
        controller.Update(camera, {.lookDelta = {0.0f, 100000.0f}}, 0.1f);
        Check(controller.GetPitch() > -XM_PIDIV2 && controller.GetPitch() < -1.5f, "lower pitch limit");
        Check(std::isfinite(camera.GetRenderView().view._11), "finite view at pitch limits");

        controller.SetPose(camera, {}, {0.0f, 0.0f, 1.0f});
        controller.Update(camera, {.movement = {0.0f, 0.0f, 1.0f}}, 5.0f);
        Check(Near(controller.GetPosition().z, 0.25f), "long frame clamped");
        controller.Update(camera, {.movement = {0.0f, 0.0f, 1.0f}}, -1.0f);
        Check(Near(controller.GetPosition().z, 0.25f), "negative time ignored");
        controller.Update(camera, {}, 0.1f);
        Check(Near(controller.GetPosition().z, 0.25f), "idle input unchanged");
        controller.ReleaseInput();
        Check(!controller.IsActive(), "release without capture is safe");
        std::cout << "FPS camera contract: PASS\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

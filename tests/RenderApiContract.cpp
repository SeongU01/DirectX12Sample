#include "pch.h"
#include "Camera.h"
#include "GeometryGenerator.h"

#include <cmath>
#include <concepts>

template <typename T> concept GenericRenderFacade =
    requires(T& core, const GeometryGenerator::MeshData& meshData, const RenderView& view, const RenderItem& item) {
    { core.CreateMesh(meshData) } -> std::same_as<MeshHandle>;
    { core.SetView(view) } -> std::same_as<void>;
    { core.Submit(item) } -> std::same_as<void>;
};

static_assert(GenericRenderFacade<GraphicsCore>);

template <typename T>
concept WorldRenderable = requires(T& item) {
    item.world;
};

static_assert(WorldRenderable<RenderItem>);

int main()
{
    constexpr float aspectRatio = 1600.0f / 900.0f;

    Camera camera;
    camera.SetLookAt({0.0f, 0.0f, -2.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    camera.SetPerspective(XM_PIDIV4, aspectRatio, 0.1f, 100.0f);

    const RenderView& view = camera.GetRenderView();
    const float projectedAspect = view.projection._11 / view.projection._22;
    return std::abs(projectedAspect - (1.0f / aspectRatio)) < 0.0001f ? 0 : 1;
}

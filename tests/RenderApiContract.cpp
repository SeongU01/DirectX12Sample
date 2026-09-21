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
    if (std::abs(projectedAspect - (1.0f / aspectRatio)) >= 0.0001f)
    {
        return 1;
    }

    // 기본 도형의 크기와 바깥쪽 와인딩을 검사해 컬링 계약을 고정한다.
    GeometryGenerator generator;
    const auto box = generator.CreateBox(1.0f, 1.0f, 1.0f, 0);
    if (box.vertices.size() != 24 || box.indices32.size() != 36)
    {
        return 2;
    }
    for (const auto& vertex : box.vertices)
    {
        if (std::abs(vertex.position.x) != 0.5f || std::abs(vertex.position.y) != 0.5f ||
            std::abs(vertex.position.z) != 0.5f || vertex.position.w != 1.0f)
        {
            return 3;
        }
    }
    for (size_t i = 0; i < box.indices32.size(); i += 3)
    {
        if (box.indices32[i] >= box.vertices.size() || box.indices32[i + 1] >= box.vertices.size() ||
            box.indices32[i + 2] >= box.vertices.size())
        {
            return 4;
        }
        const XMVECTOR a = XMLoadFloat4(&box.vertices[box.indices32[i]].position);
        const XMVECTOR b = XMLoadFloat4(&box.vertices[box.indices32[i + 1]].position);
        const XMVECTOR c = XMLoadFloat4(&box.vertices[box.indices32[i + 2]].position);
        const XMVECTOR normal = XMVector3Cross(b - a, c - a);
        if (XMVectorGetX(XMVector3Dot(normal, a + b + c)) <= 0.0f)
        {
            return 5;
        }
    }
    return 0;
}

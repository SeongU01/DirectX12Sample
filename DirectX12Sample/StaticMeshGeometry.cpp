#include "pch.h"
#include "StaticMeshGeometry.h"
#include <cmath>

StaticMeshData StaticMeshGeometry::CreateBox(float size)
{
    if (!std::isfinite(size) || size <= 0) throw std::invalid_argument("Box size must be positive");
    const auto source = GeometryGenerator{}.CreateBox(size, size, size, 0);
    StaticMeshData result;
    result.indices = source.indices32;
    for (const auto& v : source.vertices)
        result.vertices.push_back({{v.position.x, v.position.y, v.position.z}, {}, {}, v.texcoord});
    for (size_t face = 0; face < 6; ++face)
    {
        auto* vertices = result.vertices.data() + face * 4;
        const auto e1 = XMLoadFloat3(&vertices[1].position) - XMLoadFloat3(&vertices[0].position);
        const auto e2 = XMLoadFloat3(&vertices[2].position) - XMLoadFloat3(&vertices[0].position);
        const auto normal = XMVector3Normalize(XMVector3Cross(e1, e2));
        const float du1 = vertices[1].uv.x - vertices[0].uv.x, dv1 = vertices[1].uv.y - vertices[0].uv.y;
        const float du2 = vertices[2].uv.x - vertices[0].uv.x, dv2 = vertices[2].uv.y - vertices[0].uv.y;
        const float determinant = du1 * dv2 - dv1 * du2;
        const auto tangent = XMVector3Normalize((e1 * dv2 - e2 * dv1) / determinant);
        const auto bitangent = XMVector3Normalize((e2 * du1 - e1 * du2) / determinant);
        const float sign = XMVectorGetX(XMVector3Dot(XMVector3Cross(normal, tangent), bitangent)) < 0 ? -1.0f : 1.0f;
        for (size_t j = 0; j < 4; ++j)
        {
            XMStoreFloat3(&vertices[j].normal, normal);
            XMStoreFloat4(&vertices[j].tangent, XMVectorSetW(tangent, sign));
        }
    }
    return result;
}

StaticMeshData StaticMeshGeometry::CreateSphere(float radius, UINT slices, UINT stacks)
{
    if (!std::isfinite(radius) || radius <= 0 || slices < 3 || stacks < 2 || slices > 512 || stacks > 512)
        throw std::invalid_argument("Invalid sphere tessellation");
    StaticMeshData result;
    for (UINT y = 0; y <= stacks; ++y)
        for (UINT x = 0; x <= slices; ++x)
        {
            const float u = static_cast<float>(x) / slices;
            const float v = static_cast<float>(y) / stacks;
            const float theta = u * XM_2PI, phi = v * XM_PI;
            const XMFLOAT3 normal{std::sin(phi) * std::cos(theta), std::cos(phi), std::sin(phi) * std::sin(theta)};
            result.vertices.push_back({{radius * normal.x, radius * normal.y, radius * normal.z}, normal,
                                       {-std::sin(theta), 0, std::cos(theta), 1}, {u, v}});
        }
    for (UINT y = 0; y < stacks; ++y)
        for (UINT x = 0; x < slices; ++x)
        {
            const UINT a = y * (slices + 1) + x, b = a + slices + 1;
            if (y != 0) result.indices.insert(result.indices.end(), {a, a + 1, b});
            if (y != stacks - 1) result.indices.insert(result.indices.end(), {b, a + 1, b + 1});
        }
    return result;
}

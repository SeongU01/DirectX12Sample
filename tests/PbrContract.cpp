#include "pch.h"
#include "StaticMeshGeometry.h"
#include "AssetPaths.h"
#include <cmath>
#include <iostream>

namespace
{
void Check(bool condition, const char* message)
{
    if (!condition) throw std::runtime_error(message);
}
void CheckMesh(const StaticMeshData& mesh)
{
    Check(!mesh.vertices.empty() && mesh.indices.size() % 3 == 0, "triangle mesh");
    for (const auto& vertex : mesh.vertices)
    {
        const auto n = XMLoadFloat3(&vertex.normal);
        const auto t = XMLoadFloat4(&vertex.tangent);
        Check(std::abs(XMVectorGetX(XMVector3Length(n)) - 1) < 1e-4f, "unit normal");
        Check(std::abs(XMVectorGetX(XMVector3Length(t)) - 1) < 1e-4f, "unit tangent");
        Check(std::abs(XMVectorGetX(XMVector3Dot(n, t))) < 1e-4f, "orthogonal tangent frame");
        Check(std::abs(vertex.tangent.w) == 1, "tangent handedness");
    }
    for (size_t i = 0; i < mesh.indices.size(); i += 3)
    {
        for (size_t j = 0; j < 3; ++j) Check(mesh.indices[i + j] < mesh.vertices.size(), "valid index");
        const auto& a = mesh.vertices[mesh.indices[i]];
        const auto& b = mesh.vertices[mesh.indices[i + 1]];
        const auto& c = mesh.vertices[mesh.indices[i + 2]];
        const auto e1 = XMLoadFloat3(&b.position) - XMLoadFloat3(&a.position);
        const auto e2 = XMLoadFloat3(&c.position) - XMLoadFloat3(&a.position);
        const auto face = XMVector3Normalize(XMVector3Cross(e1, e2));
        Check(XMVectorGetX(XMVector3Dot(face, XMLoadFloat3(&a.normal))) > 0, "outward winding");
        const float du1 = b.uv.x - a.uv.x, dv1 = b.uv.y - a.uv.y;
        const float du2 = c.uv.x - a.uv.x, dv2 = c.uv.y - a.uv.y;
        const float det = du1 * dv2 - dv1 * du2;
        Check(std::abs(det) > 1e-7f, "nondegenerate UVs");
        const auto uvT = XMVector3Normalize((e1 * dv2 - e2 * dv1) / det);
        const auto uvB = XMVector3Normalize((e2 * du1 - e1 * du2) / det);
        const auto n = XMLoadFloat3(&a.normal), t = XMLoadFloat4(&a.tangent);
        Check(XMVectorGetX(XMVector3Dot(uvT, t)) > 0.9f, "tangent follows U");
        Check(XMVectorGetX(XMVector3Dot(uvB, XMVector3Cross(n, t) * a.tangent.w)) > 0.9f, "bitangent follows V");
    }
}
}

int main()
{
    try
    {
        CheckMesh(StaticMeshGeometry::CreateBox(1));
        CheckMesh(StaticMeshGeometry::CreateSphere(1));
        const auto relative = std::filesystem::path("Texture/dark-grey-tiles-ue/dark-grey-tiles-ue/dark-grey-tiles_albedo.png");
        const auto expected = AssetPaths::Resolve(relative);
        const auto originalDirectory = std::filesystem::current_path();
        std::filesystem::current_path(std::filesystem::temp_directory_path());
        const auto fromOtherDirectory = AssetPaths::Resolve(relative);
        std::filesystem::current_path(originalDirectory);
        Check(expected == fromOtherDirectory, "assets independent of working directory");
        bool rejected = false;
        try { AssetPaths::Resolve("../outside.png"); } catch (const std::invalid_argument&) { rejected = true; }
        Check(rejected, "reject traversal");
        rejected = false;
        try { AssetPaths::Resolve(expected); } catch (const std::invalid_argument&) { rejected = true; }
        Check(rejected, "reject absolute asset paths");
        rejected = false;
        try { AssetPaths::Resolve("missing.png"); } catch (const std::runtime_error&) { rejected = true; }
        Check(rejected, "missing assets are errors");
        std::cout << "PBR geometry and asset path contracts: PASS\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

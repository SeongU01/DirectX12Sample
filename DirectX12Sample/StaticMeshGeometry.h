#pragma once
#include "pch.h"

struct StaticMeshVertex
{
    XMFLOAT3 position;
    XMFLOAT3 normal;
    XMFLOAT4 tangent;
    XMFLOAT2 uv;
};

struct StaticMeshData
{
    std::vector<StaticMeshVertex> vertices;
    std::vector<UINT> indices;
};

namespace StaticMeshGeometry
{
    StaticMeshData CreateBox(float size);
    StaticMeshData CreateSphere(float radius, UINT slices = 64, UINT stacks = 32);
}

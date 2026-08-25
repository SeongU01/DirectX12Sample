#pragma once
using VertexBufferID = UINT;
using IndexBufferID  = UINT;
using MeshInstanceID = UINT;
using LinearColor    = std::array<float, 4>;

class BaseMesh;
using MeshHandle = std::shared_ptr<BaseMesh>;

struct RenderView
{
    XMFLOAT4X4 view{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    XMFLOAT4X4 projection{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
};

struct RenderItem
{
    MeshHandle  mesh;
    XMFLOAT4X4  world{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    LinearColor tint{1.0f, 1.0f, 1.0f, 1.0f};
};

struct DescriptorHandles
{
	D3D12_CPU_DESCRIPTOR_HANDLE CPU;
	D3D12_GPU_DESCRIPTOR_HANDLE GPU;
};

struct Vertex
{
	XMFLOAT4 Position;
	XMFLOAT2 UV;
};

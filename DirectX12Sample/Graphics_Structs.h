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

// 조명 없는 기존 샘플은 유지하고, 면의 방향을 보여줄 도형만 평면 음영을 선택한다.
enum class ShadingMode : UINT
{
    Unlit = 0,
    Flat = 1,
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
    ShadingMode shading = ShadingMode::Unlit;
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

#pragma once
using VertexBufferID = UINT;
using IndexBufferID  = UINT;
using MeshInstanceID = UINT;
using LinearColor    = std::array<float, 4>;

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

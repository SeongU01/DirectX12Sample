#include "pch.h"
#include "Renderer.h"

#include "GraphicsPipeline.h"
#include "d3dUtil.h"
#include "PbrRenderPass.h"

namespace
{
struct FrameConstants
{
    XMFLOAT4X4 view;
    XMFLOAT4X4 projection;
};

struct DrawConstants
{
    XMFLOAT4X4  world;
    LinearColor tint;
    ShadingMode shading;
};

static_assert(sizeof(DrawConstants) == sizeof(float) * 21);
static_assert(offsetof(DrawConstants, shading) == sizeof(float) * 20);
} // namespace

Renderer::Renderer() = default;
Renderer::~Renderer() = default;

void Renderer::Initialize()
{
    InitializePipeline();
    InitializeFrameConstants();
}

void Renderer::Render(const LinearColor& clearColor)
{
    const XMVECTOR color = XMVectorSet(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    Global::device->ClearBackBuffer(0, color);

    _frameItems = std::move(_renderItems);
    _renderItems.clear();
    if (!_frameItems.empty())
    {
        ID3D12GraphicsCommandList* commandList = Global::device->GetCommandList();
        _pipeline->Bind(commandList);
        UpdateFrameConstants();
        commandList->SetGraphicsRootConstantBufferView(0, _frameConstantBuffer->GetGPUVirtualAddress());

        for (const RenderItem& item : _frameItems)
        {
            if (item.material) continue;
            const DrawConstants constants{item.world, item.tint, item.shading};
            commandList->SetGraphicsRoot32BitConstants(
                1, static_cast<UINT>(sizeof(constants) / sizeof(float)), &constants, 0);
            item.mesh->Render(commandList);
        }
        if (std::any_of(_frameItems.begin(), _frameItems.end(), [](const auto& item) { return item.material != nullptr; }))
        {
            if (!_pbrPass)
            {
                auto pass = std::make_unique<PbrRenderPass>();
                pass->Initialize();
                _pbrPass = std::move(pass);
            }
            _pbrPass->Draw(commandList, _renderView, _lighting, _debugMode, _frameItems);
        }
    }
}

void Renderer::Flip()
{
    Global::device->Execute();
    Global::device->Flip();
    Global::device->ResetCommands();
    Global::device->ResetComputeCommands();
}

MeshHandle Renderer::CreateMesh(const GeometryGenerator::MeshData& meshData)
{
    if (meshData.vertices.empty() || meshData.indices32.empty())
    {
        throw std::invalid_argument("Mesh data must contain vertices and indices");
    }

    VIBuffer::Descriptor descriptor{
        .vertexData   = const_cast<GeometryGenerator::Vertex*>(meshData.vertices.data()),
        .indexData    = const_cast<GeometryGenerator::uint32*>(meshData.indices32.data()),
        .vertexSize   = static_cast<UINT>(sizeof(GeometryGenerator::Vertex) * meshData.vertices.size()),
        .vertexStride = sizeof(GeometryGenerator::Vertex),
        .indexSize    = static_cast<UINT>(sizeof(GeometryGenerator::uint32) * meshData.indices32.size()),
        .indexCount   = static_cast<UINT>(meshData.indices32.size()),
    };

    MeshHandle mesh = std::make_shared<BaseMesh>();
    mesh->Initialize(descriptor);
    return mesh;
}

void Renderer::SetView(const RenderView& view)
{
    _renderView = view;
}

MeshHandle Renderer::CreateStaticMesh(const StaticMeshData& meshData)
{
    if (meshData.vertices.empty() || meshData.indices.empty() || meshData.indices.size() % 3 != 0 ||
        meshData.vertices.size() > UINT_MAX / sizeof(StaticMeshVertex) || meshData.indices.size() > UINT_MAX / sizeof(UINT))
        throw std::invalid_argument("Invalid static mesh data");
    for (UINT index : meshData.indices)
        if (index >= meshData.vertices.size()) throw std::out_of_range("Static mesh index out of range");
    const VIBuffer::Descriptor descriptor{
        .vertexData = const_cast<StaticMeshVertex*>(meshData.vertices.data()),
        .indexData = const_cast<UINT*>(meshData.indices.data()),
        .vertexSize = static_cast<UINT>(sizeof(StaticMeshVertex) * meshData.vertices.size()),
        .vertexStride = sizeof(StaticMeshVertex),
        .indexSize = static_cast<UINT>(sizeof(UINT) * meshData.indices.size()),
        .indexCount = static_cast<UINT>(meshData.indices.size()),
    };
    auto mesh = std::make_shared<BaseMesh>();
    mesh->Initialize(descriptor);
    return mesh;
}

void Renderer::SetLighting(const PbrLighting& lighting, UINT debugMode)
{
    _lighting = lighting;
    _debugMode = debugMode;
}

void Renderer::Submit(const RenderItem& item)
{
    if (item.mesh)
    {
        _renderItems.push_back(item);
    }
}

void Renderer::InitializePipeline()
{
    wchar_t executablePath[MAX_PATH]{};
    const DWORD pathLength = GetModuleFileNameW(nullptr, executablePath, MAX_PATH);
    if (pathLength == 0 || pathLength == MAX_PATH)
    {
        throw std::runtime_error("Failed to resolve executable path");
    }

    CD3DX12_ROOT_PARAMETER rootParameters[2];
    rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
    rootParameters[1].InitAsConstants(sizeof(DrawConstants) / sizeof(float), 1, 0, D3D12_SHADER_VISIBILITY_ALL);
    CD3DX12_ROOT_SIGNATURE_DESC rootSignature(
        static_cast<UINT>(std::size(rootParameters)), rootParameters, 0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    GraphicsPipeline::Descriptor descriptor{
        .shaderPath = std::filesystem::path(executablePath).parent_path() / L"Shaders" / L"BasicColor.hlsl",
        .vertexEntry = L"VSMain",
        .vertexTarget = L"vs_6_0",
        .pixelEntry = L"PSMain",
        .pixelTarget = L"ps_6_0",
        .inputLayout = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
             static_cast<UINT>(offsetof(GeometryGenerator::Vertex, position)), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
             0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
             static_cast<UINT>(offsetof(GeometryGenerator::Vertex, texcoord)), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
             0},
        },
        .rootSignature = rootSignature,
        .topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
        .renderTargetFormat = Global::device->GetMode().Format,
        .depthStencilFormat = Global::device->GetDepthBufferFormat(),
    };

    _pipeline = std::make_unique<GraphicsPipeline>();
    _pipeline->Initialize(*Global::device, descriptor);
}

void Renderer::InitializeFrameConstants()
{
    const UINT byteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(FrameConstants));
    Global::device->CreateConstantBuffer(nullptr, byteSize, _frameConstantBuffer);
}

void Renderer::UpdateFrameConstants()
{
    const FrameConstants constants{_renderView.view, _renderView.projection};
    Global::device->UpdateBuffer(_frameConstantBuffer, &constants, sizeof(constants));
}

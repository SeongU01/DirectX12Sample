#include "pch.h"
#include "PbrRenderPass.h"
#include "AssetPaths.h"
#include "StaticMeshGeometry.h"

namespace
{
struct FrameData
{
    XMFLOAT4X4 view, projection;
    XMFLOAT4 cameraExposure, directionIntensity, colorAmbient, pointPositionIntensity, pointColor;
    UINT debugMode;
    float padding[3]{};
};
struct ObjectData
{
    XMFLOAT4X4 world, normalMatrix;
    LinearColor baseColor;
    XMFLOAT4 factors;
    XMFLOAT2 uvScale;
    UINT useTextures;
    float padding = 0;
};
static_assert(sizeof(FrameData) == 224 && sizeof(ObjectData) == 176);
constexpr UINT ConstantStride = 256;
}

void PbrRenderPass::Initialize()
{
    CD3DX12_ROOT_PARAMETER roots[7];
    roots[0].InitAsConstantBufferView(0);
    roots[1].InitAsConstantBufferView(1);
    CD3DX12_DESCRIPTOR_RANGE textures[5];
    for (UINT i = 0; i < 5; ++i)
    {
        textures[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i);
        roots[i + 2].InitAsDescriptorTable(1, &textures[i], D3D12_SHADER_VISIBILITY_PIXEL);
    }
    const CD3DX12_STATIC_SAMPLER_DESC sampler(0, D3D12_FILTER_ANISOTROPIC,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        0, 8, D3D12_COMPARISON_FUNC_ALWAYS, D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
        0, D3D12_FLOAT32_MAX, D3D12_SHADER_VISIBILITY_PIXEL);
    const CD3DX12_ROOT_SIGNATURE_DESC rootSignature(7, roots, 1, &sampler,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    const auto shaderDirectory = AssetPaths::ExecutableDirectory() / "Shaders";
    GraphicsPipeline::Descriptor descriptor{
        .shaderPath = shaderDirectory / "StaticMeshVS.hlsl",
        .vertexEntry = L"StaticMeshVS", .vertexTarget = L"vs_6_0",
        .pixelEntry = L"PbrPS", .pixelTarget = L"ps_6_0",
        .inputLayout = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(StaticMeshVertex, position), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(StaticMeshVertex, normal), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(StaticMeshVertex, tangent), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(StaticMeshVertex, uv), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        },
        .rootSignature = rootSignature,
        .renderTargetFormat = Global::device->GetMode().Format,
        .depthStencilFormat = Global::device->GetDepthBufferFormat(),
        .pixelShaderPath = shaderDirectory / "PbrPS.hlsl",
    };
    _pipeline.Initialize(*Global::device, descriptor);
    Global::device->CreateConstantBuffer(nullptr, ConstantStride, _frameBuffer);
    const std::array<std::array<unsigned char, 4>, 5> colors{{
        {255, 255, 255, 255}, {128, 128, 255, 255}, {255, 255, 255, 255},
        {255, 255, 255, 255}, {255, 255, 255, 255}}};
    for (size_t i = 0; i < _defaults.size(); ++i)
    {
        _defaults[i] = std::make_shared<Texture2D>();
        _defaults[i]->CreateSolid(colors[i], i == static_cast<size_t>(PbrTexture::Albedo));
    }
}

void PbrRenderPass::Draw(ID3D12GraphicsCommandList* commands, const RenderView& view,
                         const PbrLighting& lighting, UINT debugMode, const std::vector<RenderItem>& items)
{
    const UINT count = static_cast<UINT>(std::count_if(items.begin(), items.end(), [](const auto& item) {
        return item.material != nullptr;
    }));
    if (!count) return;
    if (count > UINT_MAX / ConstantStride) throw std::length_error("Too many PBR draws");
    if (count > _drawCapacity)
    {
        Global::device->CreateConstantBuffer(nullptr, count * ConstantStride, _drawBuffer);
        _drawCapacity = count;
    }
    XMFLOAT4 cameraPosition;
    XMStoreFloat4(&cameraPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&view.view)).r[3]);
    const FrameData frame{view.view, view.projection,
        {cameraPosition.x, cameraPosition.y, cameraPosition.z, lighting.exposure},
        {lighting.direction.x, lighting.direction.y, lighting.direction.z, lighting.intensity},
        {lighting.color.x, lighting.color.y, lighting.color.z, lighting.ambient},
        {lighting.pointPosition.x, lighting.pointPosition.y, lighting.pointPosition.z, lighting.pointIntensity},
        {lighting.pointColor.x, lighting.pointColor.y, lighting.pointColor.z, 0}, debugMode};
    Global::device->UpdateBuffer(_frameBuffer, &frame, sizeof(frame));
    std::vector<unsigned char> constants(static_cast<size_t>(count) * ConstantStride);
    UINT index = 0;
    for (const auto& item : items)
    {
        if (!item.material) continue;
        const auto& material = *item.material;
        ObjectData object{item.world, {}, material.baseColor,
            {material.metallic, material.roughness, material.normalScale, material.aoStrength},
            material.uvScale, material.useTextures ? 1u : 0u};
        const auto world = XMLoadFloat4x4(&item.world);
        if (std::abs(XMVectorGetX(XMMatrixDeterminant(world))) < 1e-8f)
            throw std::invalid_argument("PBR world transform must be invertible");
        XMStoreFloat4x4(&object.normalMatrix, XMMatrixTranspose(XMMatrixInverse(nullptr, world)));
        memcpy(constants.data() + static_cast<size_t>(index++) * ConstantStride, &object, sizeof(object));
    }
    Global::device->UpdateBuffer(_drawBuffer, constants.data(), constants.size());
    _pipeline.Bind(commands);
    ID3D12DescriptorHeap* heaps[]{Global::viewManager->GetShaderResourceHeap()};
    commands->SetDescriptorHeaps(1, heaps);
    commands->SetGraphicsRootConstantBufferView(0, _frameBuffer->GetGPUVirtualAddress());
    index = 0;
    for (const auto& item : items)
    {
        if (!item.material) continue;
        commands->SetGraphicsRootConstantBufferView(1, _drawBuffer->GetGPUVirtualAddress() + index++ * ConstantStride);
        for (UINT i = 0; i < _defaults.size(); ++i)
        {
            const auto& texture = item.material->textures[i] ? item.material->textures[i] : _defaults[i];
            commands->SetGraphicsRootDescriptorTable(i + 2, texture->GetShaderResource());
        }
        item.mesh->Render(commands);
    }
}

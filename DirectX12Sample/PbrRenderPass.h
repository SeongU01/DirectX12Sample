#pragma once
#include "pch.h"
#include "PbrMaterial.h"
#include "GraphicsPipeline.h"

class PbrRenderPass
{
public:
    void Initialize();
    void Draw(ID3D12GraphicsCommandList* commands, const RenderView& view, const PbrLighting& lighting,
              UINT debugMode, const std::vector<RenderItem>& items);

private:
    GraphicsPipeline _pipeline;
    ComPtr<ID3D12Resource> _frameBuffer;
    ComPtr<ID3D12Resource> _drawBuffer;
    UINT _drawCapacity = 0;
    std::array<TextureHandle, static_cast<size_t>(PbrTexture::Count)> _defaults;
};

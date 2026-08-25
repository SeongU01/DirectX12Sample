#include "pch.h"
#include "GraphicsPipeline.h"

#include "d3dUtil.h"

void GraphicsPipeline::Initialize(Device& device, const Descriptor& descriptor)
{
    const ComPtr<IDxcBlob> vertexShader =
        d3dUtil::CompileShader(descriptor.shaderPath.c_str(), descriptor.vertexEntry.c_str(),
                               descriptor.vertexTarget.c_str());
    const ComPtr<IDxcBlob> pixelShader =
        d3dUtil::CompileShader(descriptor.shaderPath.c_str(), descriptor.pixelEntry.c_str(),
                               descriptor.pixelTarget.c_str());

    D3D12_ROOT_SIGNATURE_DESC rootSignature = descriptor.rootSignature;
    _rootSignature = device.CreateRootSignature(rootSignature);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline{};
    pipeline.InputLayout = {
        descriptor.inputLayout.data(),
        static_cast<UINT>(descriptor.inputLayout.size()),
    };
    pipeline.pRootSignature        = _rootSignature.Get();
    pipeline.VS                    = {vertexShader->GetBufferPointer(), vertexShader->GetBufferSize()};
    pipeline.PS                    = {pixelShader->GetBufferPointer(), pixelShader->GetBufferSize()};
    pipeline.RasterizerState       = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pipeline.BlendState            = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pipeline.DepthStencilState     = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pipeline.DepthStencilState.DepthEnable   = FALSE;
    pipeline.DepthStencilState.StencilEnable = FALSE;
    pipeline.SampleMask            = UINT_MAX;
    pipeline.PrimitiveTopologyType = descriptor.topologyType;
    pipeline.NumRenderTargets      = 1;
    pipeline.RTVFormats[0]         = descriptor.renderTargetFormat;
    pipeline.SampleDesc.Count      = 1;

    const HRESULT hr =
        device.GetDevice()->CreateGraphicsPipelineState(&pipeline, IID_PPV_ARGS(_pipelineState.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"GraphicsPipeline::Initialize : CreateGraphicsPipelineState Failed");
}

void GraphicsPipeline::Bind(ID3D12GraphicsCommandList* commandList) const
{
    commandList->SetGraphicsRootSignature(_rootSignature.Get());
    commandList->SetPipelineState(_pipelineState.Get());
}

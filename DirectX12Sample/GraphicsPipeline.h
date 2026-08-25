#pragma once

class Device;

class GraphicsPipeline
{
public:
    struct Descriptor
    {
        std::filesystem::path                 shaderPath;
        std::wstring                          vertexEntry;
        std::wstring                          vertexTarget;
        std::wstring                          pixelEntry;
        std::wstring                          pixelTarget;
        std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
        D3D12_ROOT_SIGNATURE_DESC             rootSignature{};
        D3D12_PRIMITIVE_TOPOLOGY_TYPE         topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        DXGI_FORMAT                           renderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    };

    void Initialize(Device& device, const Descriptor& descriptor);
    void Bind(ID3D12GraphicsCommandList* commandList) const;

private:
    ComPtr<ID3D12RootSignature> _rootSignature;
    ComPtr<ID3D12PipelineState> _pipelineState;
};

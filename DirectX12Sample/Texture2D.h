#pragma once
#include "pch.h"

class Texture2D
{
public:
    Texture2D() = default;
    ~Texture2D();
    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    void Load(const std::filesystem::path& relativePath, bool sRGB);
    void CreateSolid(const std::array<unsigned char, 4>& color, bool sRGB);
    D3D12_GPU_DESCRIPTOR_HANDLE GetShaderResource() const { return _handles.GPU; }

private:
    void Upload(UINT width, UINT height, std::vector<unsigned char> pixels, bool sRGB);
    ComPtr<ID3D12Resource> _resource;
    DescriptorHandles _handles{};
};

using TextureHandle = std::shared_ptr<Texture2D>;

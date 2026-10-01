#include "pch.h"
#include "Texture2D.h"
#include "AssetPaths.h"
#include <wincodec.h>
#include <cmath>

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")

namespace
{
void Check(HRESULT result)
{
    if (FAILED(result)) throw std::runtime_error("Texture operation failed: " + std::to_string(result));
}

struct ComScope
{
    HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    ComScope() { if (result != RPC_E_CHANGED_MODE) Check(result); }
    ~ComScope() { if (SUCCEEDED(result)) CoUninitialize(); }
};

float Decode(float value) { return value <= 0.04045f ? value / 12.92f : std::pow((value + 0.055f) / 1.055f, 2.4f); }
float Encode(float value) { return value <= 0.0031308f ? value * 12.92f : 1.055f * std::pow(value, 1.0f / 2.4f) - 0.055f; }
}

Texture2D::~Texture2D()
{
    if (_handles.CPU.ptr && Global::viewManager)
        Global::viewManager->ReturnShaderResourceDescriptorHeap(_handles.CPU);
}

void Texture2D::Load(const std::filesystem::path& relativePath, bool sRGB)
{
    const auto file = AssetPaths::Resolve(relativePath);
    ComScope com;
    ComPtr<IWICImagingFactory> factory;
    Check(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)));
    ComPtr<IWICBitmapDecoder> decoder;
    Check(factory->CreateDecoderFromFilename(file.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder));
    ComPtr<IWICBitmapFrameDecode> frame;
    Check(decoder->GetFrame(0, &frame));
    ComPtr<IWICFormatConverter> converter;
    Check(factory->CreateFormatConverter(&converter));
    Check(converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone,
                               nullptr, 0, WICBitmapPaletteTypeCustom));
    UINT width = 0, height = 0;
    Check(frame->GetSize(&width, &height));
    if (!width || !height || width > 16384 || height > 16384)
        throw std::runtime_error("Unsupported texture dimensions: " + file.string());
    std::vector<unsigned char> pixels(static_cast<size_t>(width) * height * 4);
    Check(converter->CopyPixels(nullptr, width * 4, static_cast<UINT>(pixels.size()), pixels.data()));
    Upload(width, height, std::move(pixels), sRGB);
}

void Texture2D::CreateSolid(const std::array<unsigned char, 4>& color, bool sRGB)
{
    Upload(1, 1, std::vector<unsigned char>(color.begin(), color.end()), sRGB);
}

void Texture2D::Upload(UINT width, UINT height, std::vector<unsigned char> pixels, bool sRGB)
{
    if (_resource) throw std::logic_error("Texture2D is immutable after upload");
    // Albedo mip은 선형 공간에서 평균내고, 나머지 데이터 맵은 감마 변환하지 않는다.
    std::vector<std::vector<unsigned char>> levels;
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    UINT w = width, h = height;
    levels.push_back(std::move(pixels));
    while (w > 1 || h > 1)
    {
        const UINT nextW = std::max(1u, w / 2), nextH = std::max(1u, h / 2);
        std::vector<unsigned char> next(static_cast<size_t>(nextW) * nextH * 4);
        for (UINT y = 0; y < nextH; ++y)
            for (UINT x = 0; x < nextW; ++x)
                for (UINT c = 0; c < 4; ++c)
                {
                    float sum = 0;
                    for (UINT dy = 0; dy < 2; ++dy)
                        for (UINT dx = 0; dx < 2; ++dx)
                        {
                            float value = levels.back()[(std::min(h - 1, y * 2 + dy) * w +
                                                        std::min(w - 1, x * 2 + dx)) * 4 + c] / 255.0f;
                            sum += sRGB && c < 3 ? Decode(value) : value;
                        }
                    const float value = sRGB && c < 3 ? Encode(sum * 0.25f) : sum * 0.25f;
                    next[(y * nextW + x) * 4 + c] = static_cast<unsigned char>(std::clamp(value, 0.0f, 1.0f) * 255 + 0.5f);
                }
        levels.push_back(std::move(next));
        w = nextW; h = nextH;
    }
    w = width; h = height;
    for (const auto& level : levels)
    {
        subresources.push_back({level.data(), static_cast<LONG_PTR>(w * 4), static_cast<LONG_PTR>(w * h * 4)});
        w = std::max(1u, w / 2); h = std::max(1u, h / 2);
    }
    auto* device = Global::device->GetDevice();
    auto* commands = Global::device->GetCommandList();
    const auto format = sRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
    const auto desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, 1, static_cast<UINT16>(levels.size()));
    const CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_DEFAULT);
    Check(device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc,
                                         D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&_resource)));
    const UINT count = static_cast<UINT>(levels.size());
    const auto uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(_resource.Get(), 0, count));
    const CD3DX12_HEAP_PROPERTIES uploadHeap(D3D12_HEAP_TYPE_UPLOAD);
    ComPtr<ID3D12Resource> upload;
    Check(device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &uploadDesc,
                                         D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&upload)));
    if (!UpdateSubresources(commands, _resource.Get(), upload.Get(), 0, 0, count, subresources.data()))
        throw std::runtime_error("Texture upload failed");
    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commands->ResourceBarrier(1, &barrier);
    Global::device->UploadResource(upload);
    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _handles);
    D3D12_SHADER_RESOURCE_VIEW_DESC srv{};
    srv.Format = format;
    srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srv.Texture2D.MipLevels = count;
    device->CreateShaderResourceView(_resource.Get(), &srv, _handles.CPU);
}

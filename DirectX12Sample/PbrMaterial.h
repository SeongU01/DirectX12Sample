#pragma once
#include "Texture2D.h"

enum class PbrTexture : size_t { Albedo, Normal, Metallic, Roughness, AO, Count };

struct PbrMaterial
{
    std::array<TextureHandle, static_cast<size_t>(PbrTexture::Count)> textures;
    LinearColor baseColor{1, 1, 1, 1};
    float metallic = 1.0f;
    float roughness = 1.0f;
    float normalScale = 1.0f;
    float aoStrength = 1.0f;
    XMFLOAT2 uvScale{2, 2};
    bool useTextures = true;
};

struct PbrLighting
{
    XMFLOAT3 direction{-0.5f, -1.0f, 0.5f};
    float intensity = 3.0f;
    XMFLOAT3 color{1.0f, 0.96f, 0.90f};
    float ambient = 0.08f;
    XMFLOAT3 pointPosition{2.0f, 2.0f, -2.0f};
    float pointIntensity = 20.0f;
    XMFLOAT3 pointColor{0.65f, 0.8f, 1.0f};
    float exposure = 1.0f;
};

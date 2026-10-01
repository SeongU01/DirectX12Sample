#pragma once
#include "pch.h"
#include "PbrMaterial.h"

class ClientUI
{
public:
    void Draw(PbrMaterial& material, PbrLighting& lighting);
    XMFLOAT3 rotation{0, 25, 0};
    LinearColor clearColor{0.025f, 0.03f, 0.04f, 1};
    int debugMode = 0;
};

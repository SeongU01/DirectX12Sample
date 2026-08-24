#include "pch.h"
#include "Renderer.h"

// geometry
#include "Model.h"
#include "Quad.h"

void Renderer::Initialize()
{
    CreateDefaultResource();
}

void Renderer::Render(const LinearColor& clearColor)
{
    const XMVECTOR color = XMVectorSet(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    Global::device->ClearBackBuffer(0, color);
}

void Renderer::Flip()
{
    Global::device->Execute();
    Global::device->Flip();
    Global::device->ResetCommands();
    Global::device->ResetComputeCommands();
}

void Renderer::CreateDefaultResource()
{
    CreateDefaultGeometry();
}

void Renderer::CreateDefaultGeometry()
{
    std::unique_ptr<Quad> quad = std::make_unique<Quad>();
    quad->Initialize(-1.0f, 1.0f, 2.0f, 2.0f, 0.0f);

    std::shared_ptr<Model>    geometry;
    std::unique_ptr<BaseMesh> baseMesh;
    auto&                     resourceManager = Global::resourceManager;

    baseMesh = std::move(quad);
    geometry = std::make_shared<Model>();
    geometry->AddMesh(std::move(baseMesh));
    _defaultResource.push_back(geometry);
    resourceManager->AddResource(L"Quad", geometry);
}

void Renderer::CreateDefaultRenderTarget()
{

}

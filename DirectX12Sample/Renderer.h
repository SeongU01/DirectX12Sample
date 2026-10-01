#pragma once
#include "GeometryGenerator.h"
#include "StaticMeshGeometry.h"
#include "PbrMaterial.h"

class GraphicsPipeline;
class PbrRenderPass;

class Renderer
{
public:
    Renderer();
    ~Renderer();

public:
	void Initialize();
	void Update(const float deltaTime);
	void Render(const LinearColor& clearColor);
	void Flip();

    MeshHandle CreateMesh(const GeometryGenerator::MeshData& meshData);
    MeshHandle CreateStaticMesh(const StaticMeshData& meshData);
    void SetLighting(const PbrLighting& lighting, UINT debugMode);
    void       SetView(const RenderView& view);
    void       Submit(const RenderItem& item);

private:
    void InitializePipeline();
    void InitializeFrameConstants();
    void UpdateFrameConstants();

    std::unique_ptr<GraphicsPipeline> _pipeline;
    std::vector<RenderItem>           _renderItems;
    std::vector<RenderItem>           _frameItems;
    std::unique_ptr<PbrRenderPass>    _pbrPass;
    PbrLighting                       _lighting;
    UINT                              _debugMode = 0;
    RenderView                        _renderView;
    ComPtr<ID3D12Resource>            _frameConstantBuffer;
};

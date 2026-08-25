#pragma once
#include "GeometryGenerator.h"

class GraphicsPipeline;

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
    void       SetView(const RenderView& view);
    void       Submit(const RenderItem& item);

private:
    void InitializePipeline();
    void InitializeFrameConstants();
    void UpdateFrameConstants();

    std::unique_ptr<GraphicsPipeline> _pipeline;
    std::vector<RenderItem>           _renderItems;
    RenderView                        _renderView;
    ComPtr<ID3D12Resource>            _frameConstantBuffer;
};

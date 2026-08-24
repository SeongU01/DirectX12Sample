#pragma once
class Renderer
{
public:
    Renderer() = default;
    ~Renderer() = default;

public:
	void Initialize();
	void Update(const float deltaTime);
	void Render(const LinearColor& clearColor);
	void Flip();

private:
    void CreateDefaultResource();
    void CreateDefaultGeometry();
    void CreateDefaultTexture();
    void CreateDefaultRenderTarget();
    void CreateDefaultShader();

private:
    void RenderToBackBuffer();

private:
    std::list<std::shared_ptr<Resource>> _defaultResource;

	BaseMesh* _frameQuad = nullptr;
};

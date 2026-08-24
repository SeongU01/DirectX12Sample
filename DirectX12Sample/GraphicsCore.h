#pragma once
class GraphicsCore
{
public:
    GraphicsCore()  = default;
    ~GraphicsCore() = default;

public:
	void Initialize(HWND hwnd, UINT width, UINT height, FeatureLevel feature, bool isEditorMode, bool isRayTracing);
	// ImGui의 초기화와 프레임 처리는 엔진 내부 ImGuiLayer에 위임한다.
	bool InitializeUI(HWND window);
	void UpdateAnimation(const float deltaTime) const;
	void Update(const float deltaTime);
	void BeginUI() const;
	void EndUI() const;
	void Render(const LinearColor& clearColor) const;
	void RenderUI() const;
	void Flip() const;
	void Finalize();

	bool ProcessUIWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) const;

private:
	// UI 백엔드에서만 사용하는 디스크립터 인터페이스이므로 클라이언트에는 노출하지 않는다.
	void AllocateShaderResourceDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle,
	                                      D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle) const;
	void ReleaseShaderResourceDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle) const;
	friend class ImGuiLayer;

private:
    class Device*            _device            = nullptr;
    class Renderer*          _renderer          = nullptr;
    class ViewManager*       _viewManager       = nullptr;
    class CommandController* _commandController = nullptr;
    class ResourceManager*   _resourceManager   = nullptr;
	// GraphicsCore가 ImGui 통합 계층의 생명주기를 소유한다.
	class ImGuiLayer*         _imGuiLayer        = nullptr;
};

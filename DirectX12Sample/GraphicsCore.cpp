#include "GraphicsCore.h"
#include "pch.h"
#include "ImGuiLayer.h"

namespace Global
{
    Device*            device;
    Renderer*          renderer;
    ViewManager*       viewManager;
    CommandController* commandController;
    ResourceManager*   resourceManager;
    bool               isRayTracing = false;
} // namespace Global

void GraphicsCore::Initialize(HWND hwnd, UINT width, UINT height, FeatureLevel feature, bool isEditorMode,
                              bool isRayTracing)
{
    _device            = new Device();
    _renderer          = new Renderer();
    _viewManager       = new ViewManager();
    _commandController = new CommandController();
    _resourceManager   = new ResourceManager();

    Global::device            = _device;
    Global::renderer          = _renderer;
    Global::viewManager       = _viewManager;
    Global::commandController = _commandController;
    Global::resourceManager   = _resourceManager;
    Global::isRayTracing      = isRayTracing;

    _device->SetUpDevice(hwnd, width, height, feature);
    _viewManager->Initialize();
    _device->Initialize();
    _device->ResetCommands();
    _renderer->Initialize();
}

bool GraphicsCore::InitializeUI(HWND window)
{
	// ImGui 구현 객체는 엔진에서 생성하고 초기화 실패 시 즉시 회수한다.
	if (!_imGuiLayer)
	{
		_imGuiLayer = new ImGuiLayer;
	}

	if (_imGuiLayer->Initialize(window, *this))
	{
		return true;
	}

	delete _imGuiLayer;
	_imGuiLayer = nullptr;
	return false;
}

void GraphicsCore::BeginUI() const
{
	if (_imGuiLayer)
	{
		_imGuiLayer->BeginFrame();
	}
}

void GraphicsCore::EndUI() const
{
	if (_imGuiLayer)
	{
		_imGuiLayer->EndFrame();
	}
}

void GraphicsCore::Render(const LinearColor& clearColor) const
{
	_renderer->Render(clearColor);
}

void GraphicsCore::RenderUI() const
{
	if (_imGuiLayer)
	{
		_imGuiLayer->Render();
	}
}

void GraphicsCore::Flip() const
{
	_renderer->Flip();
}

void GraphicsCore::AllocateShaderResourceDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle,
                                                     D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle) const
{
	DescriptorHandles handles{};
	_viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, handles);
	cpuHandle = handles.CPU;
	gpuHandle = handles.GPU;
}

void GraphicsCore::ReleaseShaderResourceDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle) const
{
	_viewManager->ReturnShaderResourceDescriptorHeap(cpuHandle);
}

void GraphicsCore::Finalize()
{
	// ImGui가 참조하는 장치와 디스크립터 힙보다 UI 계층을 먼저 종료한다.
	if (_imGuiLayer)
	{
		_imGuiLayer->Finalize();
		delete _imGuiLayer;
		_imGuiLayer = nullptr;
	}

	if (_renderer)
	{
		delete _renderer;
		_renderer = nullptr;
		Global::renderer = nullptr;
	}
	if (_resourceManager)
	{
		delete _resourceManager;
		_resourceManager = nullptr;
		Global::resourceManager = nullptr;
	}
	if (_viewManager)
	{
		delete _viewManager;
		_viewManager = nullptr;
		Global::viewManager = nullptr;
	}
	if (_commandController)
	{
		delete _commandController;
		_commandController = nullptr;
		Global::commandController = nullptr;
	}
	if (_device)
	{
		_device->Finalize();
		delete _device;
		_device = nullptr;
		Global::device = nullptr;
	}
}

bool GraphicsCore::ProcessUIWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) const
{
	return _imGuiLayer && _imGuiLayer->ProcessWindowMessage(window, message, wParam, lParam);
}

#include "pch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND window, UINT message, WPARAM wParam, LPARAM lParam);

bool ImGuiLayer::Initialize(HWND window, GraphicsCore& graphicsCore)
{
    // 중복 초기화로 컨텍스트와 디스크립터가 이중 생성되는 것을 방지한다.
    if (_initialized)
    {
        return true;
    }

    _graphicsCore = &graphicsCore;

    // ImGui 공통 컨텍스트는 엔진 계층에서 한 번만 생성한다.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    _contextCreated = true;

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    if (!ImGui_ImplWin32_Init(window))
    {
        Finalize();
        return false;
    }
    _win32Initialized = true;

    // DX12 백엔드가 사용할 장치, 큐, 힙을 GraphicsCore에서 연결한다.
    ImGui_ImplDX12_InitInfo initInfo{};
    initInfo.Device                = graphicsCore._device->GetDevice();
    initInfo.CommandQueue          = graphicsCore._commandController->GetCommandQueue(CommandQueueType::GRAPHICS_QUEUE);
    initInfo.NumFramesInFlight     = SWAPCHAIN_BUFFER_COUNT;
    initInfo.RTVFormat             = graphicsCore._device->GetBackBufferFormat();
    initInfo.DSVFormat             = DXGI_FORMAT_UNKNOWN;
    initInfo.UserData              = this;
    initInfo.SrvDescriptorHeap     = graphicsCore._viewManager->GetShaderResourceHeap();
    // ImGui가 요청하는 SRV 디스크립터의 할당과 반환은 엔진의 ViewManager를 통한다.
    initInfo.SrvDescriptorAllocFn  = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* cpuHandle,
                                        D3D12_GPU_DESCRIPTOR_HANDLE* gpuHandle) {
        auto* layer = static_cast<ImGuiLayer*>(info->UserData);
        layer->_graphicsCore->AllocateShaderResourceDescriptor(*cpuHandle, *gpuHandle);
    };
    initInfo.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
                                      D3D12_GPU_DESCRIPTOR_HANDLE) {
        auto* layer = static_cast<ImGuiLayer*>(info->UserData);
        layer->_graphicsCore->ReleaseShaderResourceDescriptor(cpuHandle);
    };

    if (!ImGui_ImplDX12_Init(&initInfo))
    {
        Finalize();
        return false;
    }

    _dx12Initialized = true;
    _initialized     = true;
    return true;
}

void ImGuiLayer::BeginFrame() const
{
    if (!_initialized)
    {
        return;
    }

    // 백엔드 준비가 끝난 뒤 공통 ImGui 프레임을 시작해야 한다.
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::EndFrame() const
{
    if (_initialized)
    {
        ImGui::Render();
    }
}

void ImGuiLayer::Render() const
{
    if (!_initialized)
    {
        return;
    }

    // ImGui 텍스처가 사용하는 셰이더 가시 힙을 그리기 전에 다시 바인딩한다.
    ID3D12DescriptorHeap* heaps[] = {_graphicsCore->_viewManager->GetShaderResourceHeap()};
    ID3D12GraphicsCommandList* commandList = _graphicsCore->_device->GetCommandList();
    commandList->SetDescriptorHeaps(1, heaps);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}

void ImGuiLayer::Finalize()
{
    // 종료 중에는 새 프레임이나 메시지를 처리하지 않는다.
    _initialized = false;

    // 각 계층이 의존하는 자원이 남아 있는 순서대로 종료한다.
    if (_dx12Initialized)
    {
        ImGui_ImplDX12_Shutdown();
        _dx12Initialized = false;
    }
    if (_win32Initialized)
    {
        ImGui_ImplWin32_Shutdown();
        _win32Initialized = false;
    }
    if (_contextCreated)
    {
        ImGui::DestroyContext();
        _contextCreated = false;
    }

    _graphicsCore = nullptr;
}

bool ImGuiLayer::ProcessWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) const
{
    return _initialized && ImGui_ImplWin32_WndProcHandler(window, message, wParam, lParam) != 0;
}

#pragma once

class GraphicsCore;

// ImGui 컨텍스트와 플랫폼/렌더러 백엔드의 생명주기를 엔진에서 관리한다.
class ImGuiLayer
{
public:
    ImGuiLayer()  = default;
    ~ImGuiLayer() = default;

    // GraphicsCore가 준비한 D3D12 자원으로 ImGui 백엔드를 초기화한다.
    bool Initialize(HWND window, GraphicsCore& graphicsCore);
    // 클라이언트가 위젯을 구성할 수 있도록 ImGui 프레임 경계를 제공한다.
    void BeginFrame() const;
    void EndFrame() const;
    // 생성된 ImGui DrawData를 현재 그래픽 명령 목록에 기록한다.
    void Render() const;
    // 그래픽 자원이 해제되기 전에 백엔드와 컨텍스트를 역순으로 종료한다.
    void Finalize();

    // Win32 입력 메시지를 ImGui 플랫폼 백엔드에 전달한다.
    bool ProcessWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) const;

private:
    GraphicsCore* _graphicsCore     = nullptr;
    bool          _contextCreated   = false;
    bool          _win32Initialized = false;
    bool          _dx12Initialized  = false;
    bool          _initialized      = false;
};

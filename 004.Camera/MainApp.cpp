#include "MainApp.h"
#include "Timer.h"
#include "Application.h"
#include "ClientUI.h"

MainApp::MainApp()
{
    _timer = Timer::GetInstance();
}

MainApp::~MainApp()
{
    Finalize();
}

void MainApp::Run()
{
    MSG msg;
    _timer->Reset();
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            _timer->Tick();
            _graphicsCore->BeginUI();
            _clientUI->Draw(_sceneColor, _rotationDegrees);
            _graphicsCore->EndUI();
            // 입력 해석과 FPS 이동은 엔진에 위임하고 클라이언트는 프레임 시간만 전달한다.
            _cameraController.Update(_application->GetWindow(), _camera, _timer->DeltaTime(),
                                     _graphicsCore->GetUIInputCapture());
            // UI에서 바뀐 각도를 같은 프레임의 원점 중심 회전에 반영한다.
            XMStoreFloat4x4(&_sceneWorld, XMMatrixRotationRollPitchYaw(
                XMConvertToRadians(_rotationDegrees.x), XMConvertToRadians(_rotationDegrees.y),
                XMConvertToRadians(_rotationDegrees.z)));
            _graphicsCore->SetView(_camera.GetRenderView());
            _graphicsCore->Submit(RenderItem{
                .mesh = _sceneMesh, .world = _sceneWorld, .tint = _sceneColor, .shading = ShadingMode::Flat});
            _graphicsCore->Render(_clientUI->GetClearColor());
            _graphicsCore->RenderUI();
            _graphicsCore->Flip();
        }
    }
}

bool MainApp::Initailize(HINSTANCE hInstance)
{
    _application = Application::Create(hInstance, TEXT("004.Camera"), 1600, 900, false, true);
    if (!_application)
    {
        return false;
    }

    try
    {
        _graphicsCore = new GraphicsCore;
        _graphicsCore->Initialize(_application->GetWindow(), _application->GetWidth(), _application->GetHeight(),
                                  FeatureLevel::LEVEL_12_1, false, Global::isRayTracing);

        // 초기 자세에서도 세 면을 확인할 수 있도록 카메라만 비스듬히 배치한다.
        _cameraController.SetSettings({.moveSpeed = 2.5f, .boostMultiplier = 3.0f, .mouseSensitivity = 0.0025f});
        _cameraController.SetPose(_camera, {2.0f, 1.5f, -3.0f}, {0.0f, 0.0f, 0.0f});
        const float aspectRatio =
            static_cast<float>(_application->GetWidth()) / static_cast<float>(_application->GetHeight());
        _camera.SetPerspective(XM_PIDIV4, aspectRatio, 0.1f, 100.0f);

        // 기본 도형의 정점/인덱스 생성과 GPU 업로드는 기존 엔진 기능을 재사용한다.
        GeometryGenerator geometryGenerator;
        _sceneMesh = _graphicsCore->CreateMesh(geometryGenerator.CreateBox(1.0f, 1.0f, 1.0f, 0));

        _clientUI = std::make_unique<ClientUI>();
        if (!_graphicsCore->InitializeUI(_application->GetWindow()))
        {
            return false;
        }

        Application::SetWindowMessageHandler([this](HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
            // 포커스/캡처 해제는 ImGui가 메시지를 소비하기 전에 엔진 컨트롤러에 전달한다.
            _cameraController.ProcessWindowMessage(window, message, wParam, lParam);
            return _graphicsCore && _graphicsCore->ProcessUIWindowMessage(window, message, wParam, lParam);
        });
        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

void MainApp::Finalize()
{
    _cameraController.ReleaseInput();
    Application::SetWindowMessageHandler({});
    _clientUI.reset();
    _sceneMesh.reset();

    if (_graphicsCore)
    {
        _graphicsCore->Finalize();
        SafeDelete(_graphicsCore);
    }
    SafeDelete(_application);

    Timer::DestroyInstance();
    _timer = nullptr;
}

MainApp* MainApp::Create(HINSTANCE hInstance)
{
    MainApp* instance = new MainApp;
    if (instance->Initailize(hInstance))
    {
        return instance;
    }

    SafeDelete(instance);
    return nullptr;
}

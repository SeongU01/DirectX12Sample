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
            _clientUI->Draw(_sceneColor);
            _graphicsCore->EndUI();
            _graphicsCore->SetView(_camera.GetRenderView());
            _graphicsCore->Submit(RenderItem{.mesh = _sceneMesh, .world = _sceneWorld, .tint = _sceneColor});
            _graphicsCore->Render(_clientUI->GetClearColor());
            _graphicsCore->RenderUI();
            _graphicsCore->Flip();
        }
    }
}

bool MainApp::Initailize(HINSTANCE hInstance)
{
    _application = Application::Create(hInstance, TEXT("002.TriangleApp"), 1600, 900, false, true);
    if (!_application)
    {
        return false;
    }

    try
    {
        _graphicsCore = new GraphicsCore;
        _graphicsCore->Initialize(_application->GetWindow(), _application->GetWidth(), _application->GetHeight(),
                                  FeatureLevel::LEVEL_12_1, false, Global::isRayTracing);

        _camera.SetLookAt({0.0f, 0.0f, -2.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
        const float aspectRatio =
            static_cast<float>(_application->GetWidth()) / static_cast<float>(_application->GetHeight());
        _camera.SetPerspective(XM_PIDIV4, aspectRatio, 0.1f, 100.0f);

        GeometryGenerator::MeshData sceneGeometry;
        sceneGeometry.vertices = {
            {{0.0f, 0.577350269f, 0.0f, 1.0f}, {0.5f, 0.0f}},
            {{0.5f, -0.288675135f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, -0.288675135f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        };
        sceneGeometry.indices32 = {0, 1, 2};
        _sceneMesh = _graphicsCore->CreateMesh(sceneGeometry);

        _clientUI = std::make_unique<ClientUI>();
        if (!_graphicsCore->InitializeUI(_application->GetWindow()))
        {
            return false;
        }

        Application::SetWindowMessageHandler([this](HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
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

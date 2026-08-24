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
            _clientUI->Draw();
            _graphicsCore->EndUI();
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

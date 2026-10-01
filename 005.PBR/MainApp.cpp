#include "pch.h"
#include "MainApp.h"
#include "Application.h"
#include "Timer.h"
#include "StaticMeshGeometry.h"

MainApp::~MainApp()
{
    _controller.ReleaseInput();
    Application::SetWindowMessageHandler({});
    _material.reset();
    _sphere.reset();
    _box.reset();
    _graphics.Finalize();
    SafeDelete(_application);
    Timer::DestroyInstance();
}

void MainApp::Initialize(HINSTANCE instance)
{
    _application = Application::Create(instance, TEXT("005.PBR"), 1600, 900, false, true);
    if (!_application) throw std::runtime_error("Cannot create PBR window");
    _graphics.Initialize(_application->GetWindow(), 1600, 900, FeatureLevel::LEVEL_12_1, false, false);
    _sphere = _graphics.CreateStaticMesh(StaticMeshGeometry::CreateSphere(1.0f));
    _box = _graphics.CreateStaticMesh(StaticMeshGeometry::CreateBox(1.65f));
    _material = std::make_shared<PbrMaterial>();
    const std::filesystem::path directory = "Texture/dark-grey-tiles-ue/dark-grey-tiles-ue";
    const std::array<const char*, 5> filenames{
        "dark-grey-tiles_albedo.png", "dark-grey-tiles_normal-dx.png", "dark-grey-tiles_metallic.png",
        "dark-grey-tiles_roughness.png", "dark-grey-tiles_ao.png"};
    for (size_t i = 0; i < filenames.size(); ++i)
        _material->textures[i] = _graphics.LoadTexture(directory / filenames[i], i == 0);
    _controller.SetPose(_camera, {0, 1.3f, -6.8f}, {0, 0, 0});
    _camera.SetPerspective(XM_PIDIV4, 1600.0f / 900.0f, 0.1f, 100.0f);
    if (!_graphics.InitializeUI(_application->GetWindow())) throw std::runtime_error("Cannot initialize ImGui");
    Application::SetWindowMessageHandler([this](HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
        _controller.ProcessWindowMessage(window, message, wParam, lParam);
        return _graphics.ProcessUIWindowMessage(window, message, wParam, lParam);
    });
    _timer = Timer::GetInstance();
}

void MainApp::SubmitScene()
{
    const auto rotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(_ui.rotation.x),
        XMConvertToRadians(_ui.rotation.y), XMConvertToRadians(_ui.rotation.z));
    RenderItem item;
    item.material = _material;
    item.mesh = _box;
    XMStoreFloat4x4(&item.world, rotation * XMMatrixTranslation(-1.2f, 0, 0));
    _graphics.Submit(item);
    item.mesh = _sphere;
    XMStoreFloat4x4(&item.world, rotation * XMMatrixTranslation(1.2f, 0, 0));
    _graphics.Submit(item);
}

void MainApp::Run()
{
    _timer->Reset();
    MSG message{};
    while (true)
    {
        if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT) break;
            TranslateMessage(&message);
            DispatchMessage(&message);
            continue;
        }
        _timer->Tick();
        const auto size = _graphics.GetViewportSize();
        if (size.cx > 0 && size.cy > 0)
            _camera.SetPerspective(XM_PIDIV4, static_cast<float>(size.cx) / size.cy, 0.1f, 100.0f);
        _graphics.BeginUI();
        _ui.Draw(*_material, _lighting);
        _graphics.EndUI();
        _controller.Update(_application->GetWindow(), _camera, _timer->DeltaTime(), _graphics.GetUIInputCapture());
        _graphics.SetView(_camera.GetRenderView());
        _graphics.SetLighting(_lighting, static_cast<UINT>(_ui.debugMode));
        SubmitScene();
        _graphics.Render(_ui.clearColor);
        _graphics.RenderUI();
        _graphics.Flip();
    }
}

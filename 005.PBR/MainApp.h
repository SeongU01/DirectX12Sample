#pragma once
#include "pch.h"
#include "Camera.h"
#include "FpsCameraController.h"
#include "ClientUI.h"

class Application;
class Timer;

class MainApp
{
public:
    ~MainApp();
    void Initialize(HINSTANCE instance);
    void Run();

private:
    void SubmitScene();
    Application* _application = nullptr;
    Timer* _timer = nullptr;
    GraphicsCore _graphics;
    Camera _camera;
    FpsCameraController _controller;
    MeshHandle _sphere, _box;
    std::shared_ptr<PbrMaterial> _material;
    PbrLighting _lighting;
    ClientUI _ui;
};

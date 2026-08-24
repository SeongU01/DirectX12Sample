#pragma once
#include "pch.h"
class Timer;
class Application;
class GraphicsCore;
class ClientUI;
class MainApp
{
public:
	explicit MainApp();
	virtual ~MainApp();
public:
	void Run();

private:
	// 애플리케이션 생성 순서를 한곳에서 관리하고 실패 여부를 호출자에게 전달한다.
	bool Initailize(HINSTANCE hInstance);
	// UI 입력 연결을 끊은 뒤 클라이언트, 엔진, 창, 싱글톤 순서로 정리한다.
	void Finalize();

public:
	static MainApp* Create(HINSTANCE hInstance);

private:
	Timer* _timer = nullptr;
	Application* _application = nullptr;
	GraphicsCore* _graphicsCore = nullptr;
	// 클라이언트는 위젯 상태만 소유하고 ImGui 백엔드는 GraphicsCore가 소유한다.
	std::unique_ptr<ClientUI> _clientUI;
};

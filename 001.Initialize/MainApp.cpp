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
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			_timer->Tick();
			// 엔진이 프레임 경계를 열고 닫는 동안 클라이언트가 위젯을 구성한다.
			_graphicsCore->BeginUI();
			_clientUI->Draw();
			_graphicsCore->EndUI();
			// 장면을 먼저 기록하고 UI를 같은 명령 목록에 추가한 뒤 Present한다.
			_graphicsCore->Render(_clientUI->GetClearColor());
			_graphicsCore->RenderUI();
			_graphicsCore->Flip();
		}
	}
}

bool MainApp::Initailize(HINSTANCE hInstance)
{
	_application = Application::Create(hInstance, TEXT("DirectX12Sample"), 1600, 900, false, true);
	if (!_application)
	{
		return false;
	}

	try
	{
		_graphicsCore = new GraphicsCore;
		_graphicsCore->Initialize(_application->GetWindow(), _application->GetWidth(), _application->GetHeight(),
		                          FeatureLevel::LEVEL_12_1, false, Global::isRayTracing);

		// ClientUI는 상태와 위젯만 만들며 백엔드 초기화는 GraphicsCore가 담당한다.
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
	// 파괴된 GraphicsCore로 메시지가 전달되지 않도록 콜백을 먼저 제거한다.
	Application::SetWindowMessageHandler({});

	if (_clientUI)
	{
		_clientUI.reset();
	}
	if (_graphicsCore)
	{
		_graphicsCore->Finalize();
		SafeDelete(_graphicsCore);
	}
	SafeDelete(_application);

	// GetInstance에서 생성한 Timer를 CRT 누수 검사 전에 명시적으로 해제한다.
	Timer::DestroyInstance();
	_timer = nullptr;
}

MainApp* MainApp::Create(HINSTANCE hInstance)
{
	MainApp* pInstance = new MainApp;
	if (pInstance->Initailize(hInstance))
		return pInstance;

	SafeDelete(pInstance);
	return nullptr;
}

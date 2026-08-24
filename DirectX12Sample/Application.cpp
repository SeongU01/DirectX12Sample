#include "pch.h"
#include "Application.h"

int WINCX = 1600;
int WINCY = 900;
bool ISREADYCLIENT = false;
bool RESIZEFLAG = true;
DEVMODE _originalDevMode{};
Application::WindowMessageHandler Application::_windowMessageHandler{};

//실행되기전 디스플레이 정보로 복원
void Application::RestoreDisplay()
{
	ChangeResolution(_originalDevMode.dmPelsWidth, _originalDevMode.dmPelsHeight);
}


void Application::SaveCurrentResolution()
{
}

void Application::ChangeResolution(int width, int height)
{
}

void Application::SetWindowMessageHandler(WindowMessageHandler handler)
{
	_windowMessageHandler = std::move(handler);
}

bool Application::Initialize(HINSTANCE hInstance, const TCHAR* appName, int width, int height, bool isFullScreen, bool showCursor)
{
	_width = width;
	_height = height;
	wc.hInstance = hInstance;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//wc.hCursor = LoadCursorFromFile(TEXT("../Resources/Cursor/black.cur"));
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = appName;

	RegisterClassExW(&wc);

	if (isFullScreen)
	{
		SaveCurrentResolution();
		ChangeResolution(_width, _height);
	}

	RECT rect{ _startLeft, _startTop, _startLeft + width, _startTop + height };
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	int adjustedWidth = rect.right - rect.left;
	int adjustedHeight = rect.bottom - rect.top;

	_hInstance = hInstance;

	if (isFullScreen)
	{
		_hWnd = CreateWindowW(appName, appName, WS_POPUP | WS_SYSMENU,
			_startLeft, _startTop, adjustedWidth, adjustedHeight, NULL, NULL, hInstance, NULL);

		if (!_hWnd)
			return false;

		ShowWindow(_hWnd, SW_MAXIMIZE);
	}
	else
	{
		_hWnd = CreateWindow(appName, appName, WS_OVERLAPPEDWINDOW,
			_startLeft, _startTop, adjustedWidth, adjustedHeight, NULL, NULL, hInstance, NULL);

		if (!_hWnd)
			return false;

		ShowWindow(_hWnd, SW_SHOW);
	}
	//ShowCursor(showCursor);
	UpdateWindow(_hWnd);

	return true;
}
Application* Application::Create(HINSTANCE hInstance, const TCHAR* appName, int width, int height, bool isFullScreen, bool showCursor)
{
	Application* pInstance = new Application;
	if (pInstance->Initialize(hInstance, appName, width, height, isFullScreen, showCursor))
	{
		return pInstance;
	}
	SafeDelete(pInstance);
	return nullptr;
}

LRESULT Application::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (_windowMessageHandler && _windowMessageHandler(hWnd, message, wParam, lParam))
		return true;
#ifdef _DEBUG
#endif // _DEBUG

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		// 나중에 리사이즈 처리할 때 필요함
	//case WM_SIZE:
	//	if (ISREADYCLIENT)
	//	{
	//		WINCX = LOWORD(lParam);
	//		WINCY = HIWORD(lParam);
	//		RESIZEFLAG = true;
	//		//D12Renderer::GetInstance()->OnResize();
	//	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

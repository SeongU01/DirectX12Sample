#pragma once

class Application
{
public:
	using WindowMessageHandler = std::function<bool(HWND, UINT, WPARAM, LPARAM)>;



public:
	Application() = default;
	virtual ~Application() = default;

public:
	HINSTANCE GetInstance() const { return _hInstance; }
	HWND GetWindow() const { return _hWnd; }
	const int GetWidth() const { return _width; }
	const int GetHeight() const { return _height; }

	void RestoreDisplay();
	void SaveCurrentResolution();
	bool Initialize(HINSTANCE hInstance, const TCHAR* appName, int width, int height, bool isFullScreen, bool showCursor = true);


public:
	static void ChangeResolution(int width, int height);
	static void SetWindowMessageHandler(WindowMessageHandler handler);
	static Application* Create(HINSTANCE hInstance, const TCHAR* appName, int width, int height, bool isFullScreen = false, bool showCursor = true);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


private:
	HINSTANCE _hInstance = nullptr;
	WNDCLASSEXW wc{};
	HWND _hWnd = nullptr;
	int _width = 0;
	int _height = 0;

	const int _startLeft = 100;
	const int _startTop = 100;

	static WindowMessageHandler _windowMessageHandler;
};


#include "pch.h"
#include "MainApp.h"
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxcompiler")
#ifdef _DEBUG
#include <crtdbg.h>
#endif

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR, int)
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    try
    {
        MainApp app;
        app.Initialize(instance);
        app.Run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& error)
    {
        MessageBoxA(nullptr, error.what(), "005.PBR initialization/render error", MB_OK | MB_ICONERROR);
        return EXIT_FAILURE;
    }
}

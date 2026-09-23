#include "MainApp.h"
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxcompiler")

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new (_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

void CheckMemoryLeaks()
{
    _CrtCheckMemory();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
    //_CrtSetBreakAlloc(277);
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    MainApp* application = MainApp::Create(hInstance);
    // 부분 초기화 실패 시 nullptr 애플리케이션을 실행하지 않는다.
    if (!application)
    {
        return EXIT_FAILURE;
    }
    application->Run();

    SafeDelete(application);
    // atexit(CheckMemoryLeaks);

    return EXIT_SUCCESS;
}

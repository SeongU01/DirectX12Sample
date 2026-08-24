#pragma once
#include <stdexcept>

// 각종 유틸리티 매크로 및 함수들
#define ERROR_MESSAGE(msg) MessageBox(NULL, msg, L"Error", MB_OK | MB_ICONERROR)
#define FAILED_CHECK_MESSAGE(hr, msg)                                                                                  \
    if (FAILED(hr))                                                                                                    \
    {                                                                                                                  \
        ERROR_MESSAGE(msg);                                                                                            \
        throw std::runtime_error("DirectX operation failed");                                                          \
    }

#ifdef _DEBUG
#define GRAPHICS_ASSERT(expression, msg)                                                                               \
    if (!(expression))                                                                                                 \
    {                                                                                                                  \
        ERROR_MESSAGE(msg);                                                                                            \
        assert(false);                                                                                                 \
    }
#else
#define GRAPHICS_ASSERT(expression, msg) ((void)0)
#endif

// 싱글톤 템플릿
template <typename T>
class SingleTon
{
protected:
    SingleTon() = default;
	virtual ~SingleTon() = default;
public:
    static T* GetInstance()
    {
        if (nullptr == _instance)
        {
			_instance = new T();
        }
		return _instance;
    }

    static void DestroyInstance()
    {
        // 프로세스 수명 싱글톤도 CRT 누수 검사 전에 명시적으로 해제한다.
        delete _instance;
        _instance = nullptr;
    }

private:
	static T* _instance;
};

template <typename T>
T* SingleTon<T>::_instance = nullptr;

// 이중 free 방지용 SafeRelease 함수
// Com객체는 사용하지 않을 것
template <typename T>
void SafeDelete(T*& pointer)
{
    if (pointer)
    {
        delete pointer;
        pointer = nullptr;
    }
}
// 배열용 SafeDeleteArray 함수
template <typename T>
void SafeDeleteArray(T*& pointer)
{
    if(pointer)
    delete[] pointer;
    pointer = nullptr;
}

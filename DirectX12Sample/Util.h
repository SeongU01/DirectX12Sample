#pragma once
#define ERROR_MESSAGE(msg) MessageBox(NULL, msg, L"Error", MB_OK | MB_ICONERROR)
#define FAILED_CHECK_MESSAGE(hr, msg)                                                                                  \
    if (FAILED(hr))                                                                                                    \
    {                                                                                                                  \
        ERROR_MESSAGE(msg);                                                                                            \
        __debugbreak();                                                                                                \
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

private:
	static T* _instance;
};

template <typename T>
T* SingleTon<T>::_instance = nullptr;
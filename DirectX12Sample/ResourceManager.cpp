#include "pch.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager() {}

ResourceManager::~ResourceManager() {}

void ResourceManager::Update()
{
    std::function<void()> callback = nullptr;

    while (!_callbackQueue.empty())
    {
        if (_callbackQueue.try_pop(callback))
        {
            if (callback)
                callback();
        }
    }
}
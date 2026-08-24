#pragma once
class Resource;

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

public:
    template <typename T>
        requires(std::is_base_of_v<Resource, T>)
    std::shared_ptr<T> LoadResource(std::filesystem::path filePath, const std::function<void()>& callback = nullptr)
    {
        // 경로 정규화
        if (std::filesystem::exists(filePath))
        {
            filePath = std::filesystem::absolute(filePath);
        }

        const std::wstring key = filePath.wstring();

        // 1차 캐시 조회
        {
            std::lock_guard<std::mutex> lock(_mutex);

            auto typeIter = _resources.find(typeid(T));
            if (typeIter != _resources.end())
            {
                auto& resourceMap = typeIter->second;

                auto resourceIter = resourceMap.find(key);
                if (resourceIter != resourceMap.end())
                {
                    std::shared_ptr<Resource> resource = resourceIter->second.lock();

                    if (resource)
                    {
                        std::shared_ptr<T> typedResource = std::static_pointer_cast<T>(resource);

                        if (typedResource->IsValid())
                        {
                            if (callback)
                            {
                                callback();
                            }
                        }

                        return typedResource;
                    }
                }
            }
        }

        // 캐시에 없거나 weak_ptr가 만료된 경우 새로 로딩
        std::shared_ptr<T> newResource = std::make_shared<T>();
        newResource->LoadResource(filePath, callback);

        // 2차 확인 후 등록
        {
            std::lock_guard<std::mutex> lock(_mutex);

            auto& resourceMap = _resources[typeid(T)];

            auto resourceIter = resourceMap.find(key);
            if (resourceIter != resourceMap.end())
            {
                std::shared_ptr<Resource> existingResource = resourceIter->second.lock();

                if (existingResource)
                {
                    return std::static_pointer_cast<T>(existingResource);
                }
            }

            resourceMap[key] = newResource;
        }

        return newResource;
    }

    template <typename T>
        requires(std::is_base_of_v<Resource, T>)
    void AddResource(std::filesystem::path filePath, std::shared_ptr<T> resource)
    {
        if (resource == nullptr)
        {
            return;
        }

        if (std::filesystem::exists(filePath))
        {
            filePath = std::filesystem::absolute(filePath);
        }

        const std::wstring key = filePath.wstring();

        std::lock_guard<std::mutex> lock(_mutex);
        _resources[typeid(T)][key] = resource;
    }

    void EnqueueCallback(const std::function<void()>& callback)
    {
        if (callback)
        {
            _callbackQueue.push(callback);
        }
    }

public:
    void Update();

private:
    using ResourceMap = std::unordered_map<std::wstring, std::weak_ptr<Resource>>;

private:
    std::unordered_map<std::type_index, ResourceMap>     _resources;
    Concurrency::concurrent_queue<std::function<void()>> _callbackQueue;
    std::mutex                                           _mutex;
};
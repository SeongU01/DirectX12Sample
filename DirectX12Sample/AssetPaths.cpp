#include "pch.h"
#include "AssetPaths.h"

std::filesystem::path AssetPaths::ExecutableDirectory()
{
    std::wstring buffer(32768, L'\0');
    const DWORD length = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
    if (!length || length >= buffer.size())
        throw std::runtime_error("Cannot locate executable directory");
    buffer.resize(length);
    return std::filesystem::path(buffer).parent_path();
}

std::filesystem::path AssetPaths::Resolve(const std::filesystem::path& relativePath)
{
    if (relativePath.empty() || relativePath.has_root_path())
        throw std::invalid_argument("Asset path must be relative to Asset/");
    for (const auto& part : relativePath)
        if (part == "..") throw std::invalid_argument("Asset path cannot escape Asset/");

    for (auto directory = ExecutableDirectory(); !directory.empty(); directory = directory.parent_path())
    {
        const auto root = directory / "Asset";
        if (std::filesystem::is_directory(root))
        {
            const auto file = root / relativePath;
            if (!std::filesystem::is_regular_file(file))
                throw std::runtime_error("Missing asset: " + file.string());
            return file;
        }
        if (directory == directory.parent_path()) break;
    }
    throw std::runtime_error("Asset directory not found beside executable or its parents");
}

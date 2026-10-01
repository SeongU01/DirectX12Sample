#pragma once
#include <filesystem>

namespace AssetPaths
{
    std::filesystem::path ExecutableDirectory();
    // Asset 루트에 대한 상대경로를 사용하므로 실행 작업 디렉터리에 의존하지 않는다.
    std::filesystem::path Resolve(const std::filesystem::path& relativePath);
}

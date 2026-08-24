#pragma once
#include "pch.h"

class ClientUI
{
public:
    ClientUI() = default;
    ~ClientUI() = default;

    // 클라이언트별 위젯 구성만 담당하며 ImGui 초기화와 렌더링은 엔진에 맡긴다.
    void Draw();
    const LinearColor& GetClearColor() const { return _clearColor; }

private:
    // UI에서 편집하고 Back Buffer 초기화에 전달할 클라이언트 상태이다.
    LinearColor _clearColor{0.10f, 0.20f, 0.35f, 1.0f};
};

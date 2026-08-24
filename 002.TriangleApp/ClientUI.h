#pragma once
#include "pch.h"

class ClientUI
{
public:
    ClientUI()  = default;
    ~ClientUI() = default;

    // 클라이언트별 위젯 구성만 담당하며 ImGui 초기화와 렌더링은 엔진에 맡긴다.
    void               Draw();
    const LinearColor& GetClearColor() const { return _clearColor; }

private:
    LinearColor _clearColor{0.45f, 0.55f, 0.60f, 1.00f};
};

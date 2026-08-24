#include "ClientUI.h"

#include "imgui.h"
void ClientUI::Draw()
{
    // 엔진이 BeginUI로 시작한 프레임 안에서 클라이언트 위젯만 구성한다.
    ImGui::Begin("Initialize Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::TextUnformatted("Back Buffer");
    ImGui::ColorEdit4("Clear Color", _clearColor.data(), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar);
    ImGui::End();
}

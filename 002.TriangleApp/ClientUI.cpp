#include "ClientUI.h"
#include "imgui.h"

void ClientUI::Draw()
{
    ImGui::Begin("Triangle Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::TextUnformatted("Back Buffer");
    ImGui::ColorEdit4("Clear Color", _clearColor.data(), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar);

    // todo 삼각형의 색을 조절할 수 있는 UI 추가

    ImGui::End();
}

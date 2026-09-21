#include "ClientUI.h"
#include "imgui.h"

void ClientUI::Draw(LinearColor& sceneColor)
{
    ImGui::Begin("Triangle Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::TextUnformatted("Back Buffer");
    ImGui::ColorEdit4("Clear Color", _clearColor.data(), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar);

    ImGui::Separator();
    ImGui::TextUnformatted("Scene");
    ImGui::ColorEdit4("Triangle Color", sceneColor.data(), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar);

    ImGui::End();
}

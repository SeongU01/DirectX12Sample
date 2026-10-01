#include "pch.h"
#include "ClientUI.h"
#include "imgui.h"

void ClientUI::Draw(PbrMaterial& material, PbrLighting& lighting)
{
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(350, 0), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("PBR Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::PushItemWidth(ImGui::GetFontSize() * 15);
        ImGui::TextUnformatted("Dark grey tiles / Static mesh");
        ImGui::TextDisabled("Hold RMB outside UI to look / WASD, Q/E");
        ImGui::TextDisabled("Release RMB or Escape to show cursor");
        ImGui::SeparatorText("Material");
        ImGui::Checkbox("Use texture maps", &material.useTextures);
        ImGui::ColorEdit3("Base color", material.baseColor.data(), ImGuiColorEditFlags_Float);
        ImGui::SliderFloat("Metallic", &material.metallic, 0, 1, "%.2f");
        ImGui::SliderFloat("Roughness", &material.roughness, 0.045f, 1, "%.2f");
        ImGui::SliderFloat("Normal scale", &material.normalScale, 0, 2, "%.2f");
        ImGui::SliderFloat("AO strength", &material.aoStrength, 0, 1, "%.2f");
        ImGui::SliderFloat2("UV repeat", &material.uvScale.x, 0.25f, 8, "%.2f");
        ImGui::TextDisabled("Texture values are multiplied by sliders.");
        ImGui::Combo("View", &debugMode, "Lit\0Albedo\0World normal\0Metallic\0Roughness\0AO\0");
        ImGui::SeparatorText("Lighting");
        ImGui::SliderFloat3("Direction", &lighting.direction.x, -1, 1, "%.2f");
        ImGui::SliderFloat("Sun intensity", &lighting.intensity, 0, 8, "%.2f");
        ImGui::ColorEdit3("Sun color", &lighting.color.x, ImGuiColorEditFlags_Float);
        ImGui::SliderFloat3("Point position", &lighting.pointPosition.x, -5, 5, "%.1f");
        ImGui::SliderFloat("Point intensity", &lighting.pointIntensity, 0, 60, "%.1f");
        ImGui::SliderFloat("Ambient", &lighting.ambient, 0, 0.5f, "%.2f");
        ImGui::SliderFloat("Exposure", &lighting.exposure, 0.1f, 4, "%.2f");
        ImGui::SeparatorText("Scene");
        ImGui::SliderFloat3("Rotation", &rotation.x, -180, 180, "%.0f deg");
        if (ImGui::Button("Reset material / lights"))
        {
            const auto maps = material.textures;
            material = PbrMaterial{};
            material.textures = maps;
            lighting = PbrLighting{};
            rotation = {0, 25, 0};
            debugMode = 0;
        }
        ImGui::PopItemWidth();
    }
    ImGui::End();
}

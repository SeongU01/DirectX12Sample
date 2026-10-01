#include "ClientUI.h"
#include "imgui.h"

void ClientUI::Draw(LinearColor& sceneColor, XMFLOAT3& rotationDegrees)
{
    // 위젯과 샘플 상태만 다루며 ImGui 컨텍스트/백엔드 처리는 엔진에 맡긴다.
    if (ImGui::Begin("Box Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::PushItemWidth(ImGui::GetFontSize() * 18.0f);
        ImGui::TextUnformatted("Back Buffer");
        ImGui::ColorEdit4("Clear Color", _clearColor.data(), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar);

        ImGui::Separator();
        ImGui::TextUnformatted("Box");
        // 불투명 도형의 기본 RGB만 편집한다. 면별 밝기는 엔진 셰이더가 계산한다.
        ImGui::ColorEdit3("Box Color", sceneColor.data(), ImGuiColorEditFlags_Float);
        ImGui::TextUnformatted("Rotation (degrees)");
        ImGui::SliderFloat("X", &rotationDegrees.x, -180.0f, 180.0f, "%.1f deg", ImGuiSliderFlags_AlwaysClamp);
        ImGui::SliderFloat("Y", &rotationDegrees.y, -180.0f, 180.0f, "%.1f deg", ImGuiSliderFlags_AlwaysClamp);
        ImGui::SliderFloat("Z", &rotationDegrees.z, -180.0f, 180.0f, "%.1f deg", ImGuiSliderFlags_AlwaysClamp);
        if (ImGui::Button("Reset Rotation"))
        {
            rotationDegrees = {0.0f, 0.0f, 0.0f};
        }
        ImGui::TextDisabled("Ctrl+click a value to type.");
        ImGui::PopItemWidth();
    }
    ImGui::End();
}

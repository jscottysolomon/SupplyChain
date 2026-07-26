#include <raylib.h>
#include <imgui/rlImGui.h>
#include <imgui/imgui.h>

#include "truck_ui.hpp"

void RenderUi() {
    TruckWidget();
    ImGui::ShowIDStackToolWindow();
}
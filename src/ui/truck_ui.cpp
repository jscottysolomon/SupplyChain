#include <raylib.h>
#include <imgui/rlImGui.h>
#include <imgui/imgui.h>

#include <vector>
#include <string>

#include "truck_ui.hpp"

std::vector<std::string> ids;

struct TruckMenu {

};

void TruckWidget() {
    bool open = true;
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(0,0),ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(displaySize.x/4, displaySize.y));

    ImGui::Begin("Truck", &open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoResize 
        | ImGuiWindowFlags_NoCollapse);

    if (ImGui::BeginPopupContextItem("my popup"))
    {
        if (ImGui::Selectable("Unload until empty")) {

        }
        if (ImGui::Selectable("Unload Quantity")) {

        }
        if (ImGui::Selectable("Wait Until")) {

        }
        ImGui::EndPopup();
    }

    if(ImGui::BeginTabBar("Tabs")) {
        if (ImGui::BeginTabItem("Schedule"))
        {
            ImGui::Text("This shows and lets you edit the truck");
            if (ImGui::Button("Add")) {
                ImGui::OpenPopup("my popup");
            }
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Inventory")) {
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Whitelist")) {
            if (ImGui::BeginTable("table1", 3))
            {
                for (int row = 0; row < 4; row++)
                {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 3; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        ImGui::Text("Row %d Column %d", row, column);
                    }
                }
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    
            
    
    ImGui::End();
}
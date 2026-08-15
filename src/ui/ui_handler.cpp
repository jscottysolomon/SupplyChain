#include "ui_handler.hpp"

#include <raylib.h>
#include <imgui/rlImGui.h>
#include <imgui/imgui.h>

#include "traffic_mediator.hpp"
#include "rules.hpp"

void UiHandler::RenderUi() {
    if(truck_ == NULL) {
        truck_ = trucks_.at(truck_index_);
    }
    TruckWidget();
    ImGui::ShowIDStackToolWindow();
}

void UiHandler::TruckWidget() {
    if(truck_ == nullptr) return;

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

    ImGui::Text("ID: %d", truck_->GetId());
    ImGui::Text("Capacity: %d/%d", truck_->GetAvailability(), truck_->GetTotalCapacity());

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
            for(std::pair<int,int> inv : truck_->GetInventoryMap()) {
                ImGui::Text("%s[%d]: %d\n", organizer_->GetWidgetName(inv.first).c_str(), inv.first, inv.second);
            }

            ImGui::EndTabItem();

            if (ImGui::Button("Next")) {
                truck_index_++;
                if(truck_index_ >= trucks_.size()) {
                    truck_index_ = 0;
                }
                truck_ = trucks_.at(truck_index_);
            }
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
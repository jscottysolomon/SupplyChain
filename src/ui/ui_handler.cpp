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

    ImGui::Text("ID: %d", truck_->GetId());
    ImGui::Text("Capacity: %d/%d", truck_->GetAvailability(), truck_->GetTotalCapacity());

    if(ImGui::BeginTabBar("Tabs")) {
        if (ImGui::BeginTabItem("Schedule"))
        {
            int i = 0;
            for(Factory* f: truck_->GetStops()) {
                if(f == nullptr) continue;
                ImGui::Text("Factory ID: %d", f->GetId());
                ImGui::PushID(i);
                if (ImGui::Button("Add")) {
                    ImGui::OpenPopup("my popup");
                }
                if (ImGui::BeginPopup("my popup")) {
                    if (ImGui::Selectable("Unload Item")) {
                        Plan* p = truck_->GetPlan(f->GetId());
                        if(p != nullptr) {
                            RuleContext& context = truck_->GetContext(f->GetId());
                            p->AddTarget(new TruckWidgetAtLeast(1, truck_->GetWidgetAmount(1) + 10), new TruckLoadWidget(1));
                        }
                    }
                    if (ImGui::Selectable("Load Item")) {

                    }
                    if (ImGui::Selectable("Wait Until")) {

                    }
                    ImGui::EndPopup();
                }
                ImGui::PopID();
                ImGui::Separator();
                i++;
            }
            ImGui::Text("This shows and lets you edit the truck");
            
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
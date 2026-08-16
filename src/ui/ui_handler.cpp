#include "ui_handler.hpp"

#include <raylib.h>
#include <imgui/rlImGui.h>
#include <imgui/imgui.h>

#include "traffic_mediator.hpp"
#include "rules.hpp"

void UiHandler::RenderUi() {
  if(truck_ == nullptr && !trucks_.empty()) {
    truck_ = trucks_.at(truck_index_);
    truck_id_ = truck_->GetId();
  }
  if(factory_ == nullptr && !factories_.empty()) {
    factory_ = factories_.at(factory_index_);
    factory_id_ = factory_->GetId();
  }

  TruckWidget();
  FactoryWidget();
  ImGui::ShowIDStackToolWindow();
}

bool Style(Rule* rule, Truck* truck, Factory* factory) {
  if(rule->Evaluate(truck->GetContext(factory->GetId()))) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
    return true;
  }
  return false;
}

void StyleEnd(bool style) {
  if(style) ImGui::PopStyleColor();
}

void UiHandler::FactoryWidget() {
  if(factory_ == nullptr) return;

  bool open = true;
  ImVec2 displaySize = ImGui::GetIO().DisplaySize;
  ImGui::SetNextWindowPos(ImVec2(displaySize.x - displaySize.x/4,
      displaySize.y - displaySize.y/4),ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(displaySize.x/4, displaySize.y/4));

  ImGui::Begin("Factory", &open, ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoCollapse);
  ImGui::Text("ID: %d", factory_->GetId());
  ImGui::Text("Capacity: %d/%d", factory_->GetAvailableCapacity(), factory_->GetMaxCapacity());
  
  ImGui::End();
}

void UiHandler::TruckWidget() {
  if(truck_ == nullptr) return;

  bool open = true;
  ImVec2 displaySize = ImGui::GetIO().DisplaySize;
  ImGui::SetNextWindowPos(ImVec2(0,0),ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(displaySize.x/4, displaySize.y));

  ImGui::Begin("Truck", &open, ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoCollapse);

  ImGui::Text("ID: %d", truck_->GetId());
  ImGui::Text("Capacity: %d/%d", truck_->GetAvailableCapacity(), truck_->GetMaxCapacity());

  if(ImGui::BeginTabBar("Tabs")) {
    if (ImGui::BeginTabItem("Schedule"))
    {
      int i = 0;
      for(Factory* f: truck_->GetSchedule()) {
        if(f == nullptr) continue;

        Plan* p = truck_->GetPlan(f->GetId());
        bool style = false;

        ImGui::Text("Factory ID: %d", f->GetId());
        ImGui::PushID(i);
        if (ImGui::Button("Add")) {
          ImGui::OpenPopup("my popup");
        }

        if (ImGui::BeginPopup("my popup")) {
          if (ImGui::Selectable("Dispatch Item")) {
            if(p != nullptr) {
              int id = 1;
              RuleContext& context = truck_->GetContext(f->GetId());
              p->AddTarget(new DispatchQuantity(id, 10, truck_->GetWidgetQuantity(id)), 
                new DispatchWidget(id));
            }
          }
          if (ImGui::Selectable("Receive Item")) {
            if(p != nullptr) {
              int id = 2;
              RuleContext& context = truck_->GetContext(f->GetId());
              p->AddTarget(new ReceiveQuantity(id, 3, truck_->GetWidgetQuantity(id)), 
                new ReceiveWidgets(id));
            }
          }
          if (ImGui::Selectable("Wait Until")) {

          }   
          ImGui::EndPopup();
        }

        if(p!= nullptr) {
          for(Target* t: p->GetTargets()) {
            Rule* r = t->GetRule();
            if (auto* rule = dynamic_cast<AmountRule*>(r)) {
              // amount_rule->GetAmount(50);
            }
            if (auto* rule = dynamic_cast<DispatchQuantity*>(r)) {
              style = Style(r,truck_,f);
              ImGui::TextWrapped("Loading atleast %d %s to Truck", rule->GetAmount(),
                organizer_->GetWidgetName(rule->GetWidgetId()).c_str());
              StyleEnd(style);
              
            } else if (auto* rule = dynamic_cast<ReceiveQuantity*>(r)) {
              style = Style(r,truck_,f);
              ImGui::TextWrapped("Loading atleast %d %s to Factory", rule->GetAmount(),
                organizer_->GetWidgetName(rule->GetWidgetId()).c_str());
              StyleEnd(style);
            }
            
          }
        }
        // ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), ImVec2(0.0f, 0.0f)

        ImGui::PopID();
        ImGui::Separator();
        i++;
      }      
      ImGui::EndTabItem();
    }
    if(ImGui::BeginTabItem("Inventory")) {      
      for(std::pair<int,int> inv : truck_->GetInventoryMap()) {
        ImGui::Text("%s[%d]: %d\n", organizer_->GetWidgetName(inv.first).c_str(), inv.first, inv.second);
      }

      if (ImGui::Button("Next")) {
        truck_index_++;
        if(truck_index_ >= trucks_.size()) {
          truck_index_ = 0;
        }
        truck_ = trucks_.at(truck_index_);
      }

      ImGui::EndTabItem();
    }
    if(ImGui::BeginTabItem("Whitelist")) {
      if (ImGui::BeginTable("Inventory", 3))
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
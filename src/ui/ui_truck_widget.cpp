#include "ui_core.hpp"

#include <raylib.h>
#include <imgui/rlImGui.h>
#include <imgui/imgui.h>

#include "traffic.hpp"
#include "rules.hpp"

void GameUi::QuantityTargetTableRow(std::string str, DispatchQuantity* rule, Target* t, 
    Factory* f, std::vector<Target*>& removals) {
  ImGui::BeginGroup();
  ImGui::Text("Dispatch");
  ImGui::SameLine();
  if (ImGui::Button("-")) {
    rule->DecreaseAmount();
  }
  ImGui::SameLine();
  ImGui::Text("%d", rule->GetAmount());
  ImGui::SameLine();
  if (ImGui::Button("+")) {
    rule->IncreaseAmount();
  }
  ImGui::SameLine();
  ImGui::Text("%s", organizer_->GetWidgetName(rule->GetWidgetId()).c_str());
  ImGui::SameLine();
  if (ImGui::SmallButton("X")) {
    removals.push_back(t);
  }
  ImGui::SameLine();
  ImGui::EndGroup();
}

void GameUi::QuantityTargetTableRow(std::string str, ReceiveQuantity* rule, Target* t, 
    Factory* f, std::vector<Target*>& removals) {
  ImGui::BeginGroup();
  ImGui::Text("Release");
  ImGui::SameLine();
  str = CreateUniqueId("-", truck_, f, rule->GetWidgetId());
  if (ImGui::Button(str.c_str()))
  {
    rule->DecreaseAmount();
  }
  ImGui::SameLine();
  ImGui::Text("%d", rule->GetAmount());
  ImGui::SameLine();
  str = CreateUniqueId("+", truck_, f, rule->GetWidgetId());
  if (ImGui::Button(str.c_str())) {
    rule->IncreaseAmount();
  }
  ImGui::SameLine();
  ImGui::Text("%s", organizer_->GetWidgetName(rule->GetWidgetId()).c_str());
  ImGui::SameLine();
  str = CreateUniqueId("X", truck_, f, rule->GetWidgetId());
  if (ImGui::SmallButton(str.c_str())) {
    removals.push_back(t);
  }
  ImGui::SameLine();
  ImGui::EndGroup();
}

void GameUi::TruckScheduleFactorySection(Factory* f, Plan* p) {
  std::string str = "";
  if (p == nullptr) {
    TraceLog(LOG_ERROR, "Plan is null!");
    return;
  }
  if (ImGui::BeginPopup("Add Rule")) {
    if (ImGui::BeginMenu("Receive")) {
      for (std::pair<int,int> pair: f->GetInventoryMap()) {
        std::string name = organizer_->GetWidgetName(pair.first) + "##" +
            std::to_string(pair.first);
        if (ImGui::MenuItem(name.c_str())) {
            p->AddTarget(new ReceiveQuantity(pair.first, 5), new ReceiveWidget(pair.first));
            break;
        }
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Dispatch")) {
      for (std::pair<int,int> pair: truck_->GetInventoryMap()) {
        std::string name = organizer_->GetWidgetName(pair.first) + "##" +
          std::to_string(pair.first);
        if (ImGui::MenuItem(name.c_str())) {
          p->AddTarget(new DispatchQuantity(pair.first, 5), new DispatchWidget(pair.first));
          break;
        }
      }
      ImGui::EndMenu();
    }
    if (ImGui::Selectable("Wait Until")) {

    }   
    ImGui::EndPopup();
  }
}

void GameUi::TruckScheduleTab() {
  if (ImGui::BeginTabItem("Schedule")) {
    int i = 0;
    for (Junction* junc : truck_->GetSchedule()) {
      Factory* factory = junc->GetFactory();
      if(factory == nullptr) 
        { continue; }
      if (factory == nullptr) continue;

      Plan* p = truck_->GetPlan(factory->GetId());
      RuleContext& context = truck_->GetContext(factory->GetId());
      bool style = false;
      std::string str = "";
      std::vector<Target*> removals;

      ImGui::Text("Factory ID: %d", factory->GetId());
      ImGui::PushID(i);
      if (ImGui::Button("Add")) {
        ImGui::OpenPopup("Add Rule");
      }
      TruckScheduleFactorySection(factory,p);

      if (p!= nullptr) {
        int ii = 0;
        for (Target* t: p->GetTargets()) {
          ImGui::PushID(t);
          TargetTableRow(t,factory,removals);
          ImGui::PopID();
        }

        for (Target* t: removals) {
          truck_->RemoveTarget(t,factory->GetId());
        }
      }

      ImGui::PopID();
        // ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), ImVec2(0.0f, 0.0f)
      ImGui::Separator();
      i++;
    }      
    ImGui::EndTabItem();
  }
}

void GameUi::TargetTableRow(
    Target* target,
    Factory* factory,
    std::vector<Target*>& removals) {
  if (target == nullptr) return;

  Rule* rule = target->GetRule();
  if (rule == nullptr) return;

  if (auto* dispatch = dynamic_cast<DispatchQuantity*>(rule)) {
    QuantityTargetTableRow("Dispatch", dispatch, target, factory, removals);
  } else if (auto* receive = dynamic_cast<ReceiveQuantity*>(rule)) {
    QuantityTargetTableRow("Receive", receive, target, factory, removals);
  }
}

void GameUi::TruckWidget() {
  if (truck_ == nullptr) return;

  bool open = true;
  ImVec2 displaySize = ImGui::GetIO().DisplaySize;
  ImGui::SetNextWindowPos(ImVec2(0,0),ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(displaySize.x/4, displaySize.y));

  ImGui::Begin("Truck", &open, ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoCollapse);

  ImGui::Text("ID: %d", truck_->GetId());
  ImGui::Text("Capacity: %d/%d", truck_->GetAvailableCapacity(), truck_->GetMaxCapacity());

  //TODO: use table for displaying targets & functions to reduce redundancy
  if (ImGui::BeginTabBar("Tabs")) {
    // TruckScheduleTab();
    if (ImGui::BeginTabItem("Inventory")) {      
      for (std::pair<int,int> inv : truck_->GetInventoryMap()) {
        ImGui::Text("%s[%d]: %d\n", organizer_->GetWidgetName(inv.first).c_str(), inv.first, inv.second);
      }
      if (ImGui::Button("Next")) {
        truck_index_++;
        if (truck_index_ >= trucks_.size()) {
          truck_index_ = 0;
        }
        truck_ = trucks_.at(truck_index_);
      }
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Whitelist")) {
      if (ImGui::BeginTable("Inventory", 3)) {
        for (int row = 0; row < 4; row++) {
          ImGui::TableNextRow();
          for (int column = 0; column < 3; column++) {
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
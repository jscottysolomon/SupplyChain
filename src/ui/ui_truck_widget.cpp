#include "ui_core.hpp"

#include <raylib.h>
#include <imgui/rlImGui.h>
#include <imgui/imgui.h>

#include "factory.hpp"
#include "traffic.hpp"
#include "rules.hpp"

void GameUi::ReceiveWidgetPalletQuantity(Rule& rule, Target& target, std::vector<Target>& removals) {
  if (ImGui::BeginTable("Table", 6,ImGuiTableFlags_SizingFixedFit)) {
    ImGui::TableSetupColumn("Type",   ImGuiTableColumnFlags_WidthFixed, 70.0f);
    ImGui::TableSetupColumn("Minus",  ImGuiTableColumnFlags_WidthFixed, 25.0f);
    ImGui::TableSetupColumn("Amount", ImGuiTableColumnFlags_WidthFixed, 25.0f);
    ImGui::TableSetupColumn("Plus",   ImGuiTableColumnFlags_WidthFixed, 25.0f);
    ImGui::TableSetupColumn("Item",   ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Remove", ImGuiTableColumnFlags_WidthFixed, 25.0f);

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
      ImGui::Text("Receive");
    ImGui::TableNextColumn();
      if (ImGui::Button("-")) 
      { rule.DecreaseAmount(); }
    ImGui::TableNextColumn();
      ImGui::Text("%d", rule.GetAmount());
    ImGui::TableNextColumn();
      if (ImGui::Button("+") )
      { rule.IncreaseAmount(); }
    ImGui::TableNextColumn();
      ImGui::Text("%s", organizer_->GetWidgetName(rule.GetWidgetId()).c_str());
    ImGui::TableNextColumn();
      if (ImGui::SmallButton("X")) 
      { removals.push_back(target);}
    ImGui::EndTable();
  }
}

void GameUi::DispatchWidgetPalletQuantity(Rule& rule, Target& target, std::vector<Target>& removals) {
  if (ImGui::BeginTable("Table", 6,ImGuiTableFlags_SizingFixedFit)) {
    ImGui::TableSetupColumn("Type",   ImGuiTableColumnFlags_WidthFixed, 70.0f);
    ImGui::TableSetupColumn("Minus",  ImGuiTableColumnFlags_WidthFixed, 25.0f);
    ImGui::TableSetupColumn("Amount", ImGuiTableColumnFlags_WidthFixed, 25.0f);
    ImGui::TableSetupColumn("Plus",   ImGuiTableColumnFlags_WidthFixed, 25.0f);
    ImGui::TableSetupColumn("Item",   ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Remove", ImGuiTableColumnFlags_WidthFixed, 25.0f);

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
      ImGui::Text("Dispatch");
    ImGui::TableNextColumn();
      if (ImGui::Button("-")) 
      { rule.DecreaseAmount(); }
    ImGui::TableNextColumn();
      ImGui::Text("%d", rule.GetAmount());
    ImGui::TableNextColumn();
      if (ImGui::Button("+") )
      { rule.IncreaseAmount(); }
    ImGui::TableNextColumn();
      ImGui::Text("%s", organizer_->GetWidgetName(rule.GetWidgetId()).c_str());
    ImGui::TableNextColumn();
      if (ImGui::SmallButton("X")) 
      { removals.push_back(target);}
    ImGui::EndTable();
  }
}

void GameUi::PlanRuleMenu(Factory* fact, Plan plan) {
  std::string str = "";
  

  Truck* truck = commander_.GetTruck(truck_id_);
  if (!truck || !fact) {
    TraceLog(LOG_WARNING, "Null truck for table rows");
    return;
  }

  if (ImGui::BeginPopup("Add Rule")) {
    if (ImGui::BeginMenu("Receive")) {
      for (std::pair<int,int> pair: fact->GetInventoryMap()) {
        std::string name = organizer_->GetWidgetName(pair.first) + "##" +
            std::to_string(pair.first);
        if (ImGui::MenuItem(name.c_str())) {
          logisticsController_.AddPlanTarget(truck_id_,fact->GetId(),
            RuleType::kReceiveWidgetPalletQuantity,pair.first,1,1,
            ActionType::kReceiveWidget);
        }
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Dispatch")) {
      for (std::pair<int,int> pair: truck->GetInventoryMap()) {
        std::string name = organizer_->GetWidgetName(pair.first) + "##" +
          std::to_string(pair.first);
        if (ImGui::MenuItem(name.c_str())) {
          logisticsController_.AddPlanTarget(truck_id_,fact->GetId(),
            RuleType::kDispatchWidgetPalletQuantity,pair.first,1,1,
            ActionType::kDispatchWidget);
        }
      }
      ImGui::EndMenu();
    }
//     if (ImGui::Selectable("Wait Until")) {

//     }   
    ImGui::EndPopup();
  }
}

void GameUi::TruckScheduleTab() {
  if (ImGui::BeginTabItem("Schedule")) {
    int i = 0;
    Truck* truck = commander_.GetTruck(truck_id_);
    if (!truck) {
      TraceLog(LOG_WARNING, "Null truck for table rows");
      return;
    }
    for (int id : truck->GetDynamicSchedule()) {
      Junction* junc = commander_.GetJunction(id);
      if(!junc) continue;

      Factory* factory = junc->GetFactory();
      if(!factory) { 
        TraceLog(LOG_WARNING,"Null factory in junction list of truck");
        continue; 
      }
      
      ImGui::Text("Factory ID: %d", factory->GetId());

      if(truck->GetPlan(factory->GetId()) <= -1) {
        logisticsController_.CreatePlan(truck,factory);
      }

      Plan& plan = logisticsController_.GetPlan(truck->GetId(), factory->GetId());

      RuleContext context = plan.GetContext();
      bool style = false;
      std::string str = "";
      std::vector<Target> removals;

      ImGui::PushID(i);
      if (ImGui::Button("Add")) {
        ImGui::OpenPopup("Add Rule");
      }
      PlanRuleMenu(factory,plan);

      int ii = 0;
      for (Target& target: plan.GetTargets()) {
        ImGui::PushID(ii);
        TargetTableRow(target,factory,removals);
        ImGui::PopID();
        ii++;
      }

      for (Target& t: removals) {
        plan.RemoveTarget(t);
      }
      

      ImGui::PopID();
        // ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), ImVec2(0.0f, 0.0f)
      ImGui::Separator();
      i++;
    }      
    if(ImGui::Button("Add Stop")) {
      truck = commander_.GetTruck(truck_id_);
      Factory* factory = commander_.GetFactory(factory_id_);
      if(truck && factory) {
        Junction* junc = commander_.GetJunction(factory->GetJunctionEntityId());
        if (junc) {
          logisticsController_.AddStopToTruck(truck_id_, junc->GetId());
        }
      }
    }
    ImGui::EndTabItem();
  }
}

void GameUi::TargetTableRow(Target& target, Factory* factory, std::vector<Target>& removals) {
  Rule& rule = target.GetRule();
  switch(rule.GetType()) {
    case RuleType::kReceiveWidgetPalletQuantity:
      ReceiveWidgetPalletQuantity(rule,target,removals);
      break;
    default:
      break;
  }

}

void GameUi::TruckWidget() {
  Truck* truck = commander_.GetTruck(truck_id_);
  if (!truck) {
    TraceLog(LOG_WARNING, "Null truck for table rows");
    return;
  }

  bool open = true;
  ImVec2 displaySize = ImGui::GetIO().DisplaySize;
  ImGui::SetNextWindowPos(ImVec2(0,0),ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(displaySize.x/4, displaySize.y/2));

  // ImGui::Begin("Truck", &open, ImGuiWindowFlags_NoResize |
  //     ImGuiWindowFlags_NoResize |
  //     ImGuiWindowFlags_NoCollapse & ~(ImGuiWindowFlags_NoMove));

  ImGui::Begin("Truck", &open);

  ImGui::Text("ID: %d", truck->GetId());
  ImGui::Text("Capacity: %d/%d", truck->GetAvailableCapacity(), truck->GetMaxCapacity());

  if (ImGui::BeginTabBar("Tabs")) {
    TruckScheduleTab();
    if (ImGui::BeginTabItem("Inventory")) {      
      for (std::pair<int,int> inv : truck->GetInventoryMap()) {
        ImGui::Text("%s[%d]: %d\n", organizer_->GetWidgetName(inv.first).c_str(), inv.first, inv.second);
      }
      if (ImGui::Button("Next")) {
        Truck* next = commander_.GetNextTruckOrFirst(truck_id_);
        if(next) {
          truck_id_ = next->GetId();
        }
      }
      ImGui::EndTabItem();
    }
    // if (ImGui::BeginTabItem("Whitelist")) {
    //   if (ImGui::BeginTable("Inventory", 3)) {
    //     for (int row = 0; row < 4; row++) {
    //       ImGui::TableNextRow();
    //       for (int column = 0; column < 3; column++) {
    //         ImGui::TableSetColumnIndex(column);
    //         ImGui::Text("Row %d Column %d", row, column);
    //       }
    //     }
    //     ImGui::EndTable();
    //   }
    //   ImGui::EndTabItem();
    // }
    ImGui::EndTabBar();
  }
  ImGui::End();
}
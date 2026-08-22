#include "ui_handler.hpp"

#include <raylib.h>
#include <imgui/rlImGui.h>
#include <imgui/imgui.h>

#include "traffic_mediator.hpp"
#include "rules.hpp"

void UiHandler::RenderUi() {
  if (truck_ == nullptr && !trucks_.empty()) {
    truck_ = trucks_.at(truck_index_);
    truck_id_ = truck_->GetId();
  }
  if (factory_ == nullptr && !factories_.empty()) {
    factory_ = factories_.at(factory_index_);
    factory_id_ = factory_->GetId();
  }

  BaseLayer();
  TruckWidget();
  FactoryWidget();
  ImGui::ShowIDStackToolWindow();
}

void FillTable() {

}

bool Style(Rule* rule, Truck* truck, Factory* factory) {
  if (rule->Evaluate(truck->GetContext(factory->GetId()))) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
    return true;
  }
  return false;
}

std::string CreateUniqueId(std::string name, Truck* t, Factory* f, int id) {
  std::string str = name + "##" + "T" + std::to_string(t->GetId()) +  
      "F" + std::to_string(f->GetId()) + "W" + std::to_string(id);

  return str;
}

std::string CreateUniqueId(std::string name, int id) {
  std::string str = name + "##" + std::to_string(id);

  return str;
}

void StyleEnd(bool style) {
  if (style) ImGui::PopStyleColor();
}

void UiHandler::BaseLayer() {
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));

  ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoScrollWithMouse |
      ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoBackground |
      ImGuiWindowFlags_NoBringToFrontOnFocus |
      ImGuiWindowFlags_NoFocusOnAppearing |
      ImGuiWindowFlags_NoNavFocus;

  ImGui::Begin("GameOverlay", nullptr, flags);

  for (Factory* factory: factories_) {
    if(factory == nullptr) {
      TraceLog(LOG_WARNING, "Null factory in factory UI list!");
      continue;
    }

    std::string str = CreateUniqueId("button", factory->GetId());
    ImGui::SetCursorScreenPos(ImVec2(factory->GetPosition().x, 
        factory->GetPosition().y));
    ImGui::InvisibleButton(str.c_str(), ImVec2(FACTORY_WIDTH*1.5, FACTORY_WIDTH*1.5));

    if (ImGui::IsItemClicked()) {
      factory_ = factory;
      if (factory_->GetId() != factory->GetId()) {
        factory_ = factory;
        auto it = find(factories_.begin(), factories_.end(), factory);
        factory_index_ = it - factories_.begin();
        break;
      }
    }
  }

  for (Truck* truck : trucks_) {
    if(truck == nullptr) {
      TraceLog(LOG_WARNING, "Null truck in truck UI list!");
      continue;
    }

    if(truck_ == nullptr) {
      TraceLog(LOG_WARNING, "Null current truck!");
      break;
    }

    std::string str = CreateUniqueId("button", truck->GetId());
    ImGui::SetCursorScreenPos(ImVec2(truck->GetPosition().x, 
        truck->GetPosition().y));
    ImGui::InvisibleButton(str.c_str(), ImVec2(TRUCK_WIDTH, TRUCK_WIDTH));

    if (ImGui::IsItemClicked()) {
      if (truck_->GetId() != truck->GetId()) {
        truck_ = truck;
        auto it = find(trucks_.begin(), trucks_.end(), truck);
        truck_index_ = it - trucks_.begin();
        break;
      }
    }
  }
  ImGui::End();
}

void UiHandler::FactoryWidget() {
  if (factory_ == nullptr) return;

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

  for (std::pair<int,int> inv : factory_->GetInventoryMap()) {
    ImGui::Text("%s[%d]: %d\n", organizer_->GetWidgetName(inv.first).c_str(), inv.first, inv.second);
  }

  if (ImGui::Button("Next")) {
    factory_index_++;
    if (factory_index_ >= factories_.size()) {
      factory_index_ = 0;
    }
    factory_ = factories_.at(factory_index_);
  }
  
  ImGui::End();
}

void UiHandler::TruckWidget() {
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
    if (ImGui::BeginTabItem("Schedule")) {
      int i = 0;
      for (Factory* f: truck_->GetSchedule()) {
        if (f == nullptr) continue;

        Plan* p = truck_->GetPlan(f->GetId());
        RuleContext& context = truck_->GetContext(f->GetId());
        bool style = false;
        std::string str = "";
        std::vector<Target*> removals_;

        ImGui::Text("Factory ID: %d", f->GetId());
        ImGui::PushID(i);
        if (ImGui::Button("Add")) {
          ImGui::OpenPopup("my popup");
        }

        if (ImGui::BeginPopup("my popup")) {
          if (ImGui::BeginMenu("Receive") && p != nullptr) {
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
          if (ImGui::BeginMenu("Dispatch") && p != nullptr) {
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

        if (p!= nullptr) {
          for (Target* t: p->GetTargets()) {
            Rule* r = t->GetRule();
            if (auto* rule = dynamic_cast<AmountRule*>(r)) {
              // amount_rule->GetAmount(50);
            }
            if (auto* rule = dynamic_cast<DispatchQuantity*>(r)) {
              // style = Style(r,truck_,f);
              // ImGui::TextWrapped("Dispatching %d %s", rule->GetAmount(),
              //   organizer_->GetWidgetName(rule->GetWidgetId()).c_str());
              // StyleEnd(style);


              ImGui::BeginGroup();
              ImGui::Text("Dispatch");
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
                removals_.push_back(t);
              }
              ImGui::SameLine();
              ImGui::EndGroup();
              
            } else if (auto* rule = dynamic_cast<ReceiveQuantity*>(r)) {
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
                removals_.push_back(t);
              }
              ImGui::SameLine();
              ImGui::EndGroup();
            }
            
          }

          //removals
          for (Target* t: removals_) {
            truck_->RemoveTarget(t,f->GetId());
          }
        }
        // ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), ImVec2(0.0f, 0.0f)

        ImGui::PopID();
        ImGui::Separator();
        i++;
      }      
      ImGui::EndTabItem();
    }
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
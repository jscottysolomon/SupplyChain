#include "ui_core.hpp"

#include <raylib.h>
#include <imgui/rlImGui.h>
#include <imgui/imgui.h>

#include "traffic.hpp"
#include "rules.hpp"

void GameUi::BaseLayer() {
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

    std::string str = CreateUniqueId("factory_button", factory->GetId());
    ImGui::SetCursorScreenPos(ImVec2(factory->GetPosition().x, 
        factory->GetPosition().y));
    ImGui::InvisibleButton(str.c_str(), ImVec2(FACTORY_WIDTH, FACTORY_WIDTH));

    if (ImGui::IsItemClicked()) {
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

    std::string str = CreateUniqueId("truck_button", truck->GetId());
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
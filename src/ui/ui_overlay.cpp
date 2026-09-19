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

  commander_.ForEachFactory([this](int id, Factory& factory) {
    std::string str = CreateUniqueId("factory_button", factory.GetId());
    ImGui::SetCursorScreenPos(ImVec2(factory.GetPosition().x, 
        factory.GetPosition().y));
    ImGui::InvisibleButton(str.c_str(), ImVec2(FACTORY_WIDTH, FACTORY_WIDTH));

    if (ImGui::IsItemClicked()) {
      if (factory_id_ != factory.GetId()) {
        factory_id_ = factory.GetId();
        last_clicked_factory_id_ = factory_id_;
      }
    }
  });

  commander_.ForEachTruck([this](int id, Truck& truck) {
    std::string str = CreateUniqueId("truck_button", truck.GetId());
    ImGui::SetCursorScreenPos(ImVec2(truck.GetPosition().x, 
        truck.GetPosition().y));
    ImGui::InvisibleButton(str.c_str(), ImVec2(TRUCK_WIDTH, TRUCK_WIDTH));

    if (ImGui::IsItemClicked()) {
      if (truck_id_ != truck.GetId()) {
        truck_id_ = truck.GetId();
      }
    }
  
  });

  ImGui::End();    
}
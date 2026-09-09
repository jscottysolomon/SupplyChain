#include "ui_core.hpp"

#include <raylib.h>
#include <imgui/rlImGui.h>
#include <imgui/imgui.h>

#include "traffic.hpp"
#include "rules.hpp"

void GameUi::RenderUi() {
  if(factory_id_ <= -1) {
    Factory* next = commander_.GetNextFactoryOrFirst(factory_id_);
    if(next) {
      factory_id_ = next->GetId();
    }
  }
  if(truck_id_ <= -1) {
    Truck* next = commander_.GetNextTruckOrFirst(truck_id_);
    if(next) {
      truck_id_ = next->GetId();
    }
  }
  
  // BaseLayer();
  // TruckWidget();
  FactoryWidget();
  ImGui::ShowIDStackToolWindow();
}

void FillTable() {

}
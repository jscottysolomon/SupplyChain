#include "ui_core.hpp"

#include <raylib.h>
#include <imgui/rlImGui.h>
#include <imgui/imgui.h>

#include "traffic.hpp"
#include "rules.hpp"

void GameUi::RenderUi() {
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
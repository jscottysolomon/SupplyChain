#include "ui_core.hpp"

#include <raylib.h>
#include <imgui/rlImGui.h>
#include <imgui/imgui.h>

#include "traffic.hpp"
#include "rules.hpp"


void GameUi::FactoryWidget() {
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

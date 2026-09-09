#include "ui_core.hpp"

#include <raylib.h>
#include <imgui/rlImGui.h>
#include <imgui/imgui.h>

#include "traffic.hpp"
#include "rules.hpp"


void GameUi::FactoryWidget() {
  Factory* factory = commander_.GetFactory(factory_id_);
  if (!factory) {
    TraceLog(LOG_WARNING, "Null factory for widget");
    return;
  }

  bool open = true;
  ImVec2 displaySize = ImGui::GetIO().DisplaySize;
  ImGui::SetNextWindowPos(ImVec2(displaySize.x - displaySize.x/4,
      displaySize.y - displaySize.y/4),ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(displaySize.x/4, displaySize.y/4));

  ImGui::Begin("Factory", &open, ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoCollapse);
  ImGui::Text("ID: %d", factory->GetId());
  ImGui::Text("Capacity: %d/%d", factory->GetAvailableCapacity(), factory->GetMaxCapacity());

  for (std::pair<int,int> inv : factory->GetInventoryMap()) {
    ImGui::Text("%s[%d]: %d\n", organizer_->GetWidgetName(inv.first).c_str(), inv.first, inv.second);
  }

  if (ImGui::Button("Next")) {
    Factory* next = commander_.GetNextFactoryOrFirst(factory_id_);
    if(next) {
      factory_id_ = next->GetId();
    }
  }
  
  ImGui::End();
}

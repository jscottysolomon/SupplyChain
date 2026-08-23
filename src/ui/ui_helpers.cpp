#include "ui_core.hpp"

#include <raylib.h>
#include <imgui/rlImGui.h>
#include <imgui/imgui.h>

#include "traffic_mediator.hpp"
#include "rules.hpp"

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
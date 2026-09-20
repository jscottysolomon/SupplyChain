#include "save.hpp"

#include <string>
#include <fstream>

#include <nlohmann/json.hpp>
#include <raylib.h>

#include "truck.hpp"
#include "traffic.hpp"
#include "factory.hpp"



std::vector<int> SerializeList(std::vector<Junction*> junctions) {
  std::vector<int> ids;
  for(Junction* obj: junctions) {
    ids.push_back(obj->GetId());
  }
  return ids;
}

std::vector<int> SerializeList(std::vector<Dock*> docks) {
  std::vector<int> ids;
  for(Dock* obj: docks) {
    ids.push_back(obj->id);
  }
  return ids;
}

std::vector<int> SerializeList(std::vector<Truck*> docks) {
  std::vector<int> ids;
  for(Truck* obj: docks) {
    ids.push_back(obj->GetId());
  }
  return ids;
}

// void to_json(nlohmann::json& j, const Vector2 vec) {
//   j = nlohmann::json{
//     {"x", vec.x},
//     {"y", vec.y}
//   };
// }

// void to_json(nlohmann::json& j, const Inventory* inventory) {
//   j = nlohmann::json{
//     {"maxCapacity", inventory->GetMaxCapacity()},
//     {"blacklist", inventory->GetWhitelist()},
//     {"whitelist", inventory->GetWhitelist()},
//     {"widgets", inventory->GetInventoryMap()}
//   };
// }

// void to_json(nlohmann::json& j, const Junction& junc) {
//   j = nlohmann::json{
//     {"entityId", junc.GetId()},
//     {"trafficEntityId", junc.GetEntityId()},
//     {"position", junc.GetPosition()}
//   };
// }

// void to_json(nlohmann::json& j, const ProductionLine line) {
//   j = nlohmann::json{
//     {"lastProduction", line.last_production},
//     {"productionStart", line.production_start},
//     {"efficiency", line.efficienty},
//     {"entityId", line.id}
//   };
// }

// void to_json(nlohmann::json& j, const FourWayStop* fourway) {
//   j = nlohmann::json{
//     {"id", fourway->GetId()},
//     {"position", fourway->GetPosition()},
//     {"laneNumber", fourway->GetLaneNumber()},
//     {"junctionId", fourway->GetJunctionEntityId()},
//     {"trucks", SerializeList(fourway->GetTrucks())}
//   };
// }

// void to_json(nlohmann::json& j, const CenterYield* yield) {
//   j = nlohmann::json{
//     {"id", yield->GetId()},
//     {"position", yield->GetPosition()},
//     {"laneNumber", yield->GetLaneNumber()},
//     {"junctionId", yield->GetJunctionEntityId()},
//     {"trucks", SerializeList(yield->GetTrucks())}
//   };
// }


// void to_json(nlohmann::json& j, const Dock* dock) {
//   j = nlohmann::json{
//     {"id", dock->id},
//     {"position", dock->position},
//     {"isAssigned", dock->assigned},
//     {"cargoReady", dock->cargo_ready},
//     {"truckId", dock->truck_id}
//   };
// }

// void to_json(nlohmann::json& j, const Factory* factory) {
//   j = nlohmann::json{
//     {"entityId", factory->GetId()},
//     {"position", factory->GetPosition()},
//     {"dockMax", factory->GetDockMaximum()},
//     {"inventory", factory->GetInventory()},
//     {"productionLines", factory->GetProductionLines()},
//     {"docks", SerializeList(factory->GetDocks())}
//   };
// }

void WriteSave(TrafficCommand& commander) {
  // std::ifstream source_file("config.json");

  nlohmann::json save;

  nlohmann::json trucks = nlohmann::json::array();

  commander.ForEachTruck([&](int id, Truck& truck){
    trucks.push_back(truck);
  }); save["trucks"] = trucks;

  nlohmann::json junctions = nlohmann::json::array();
  nlohmann::json factories = nlohmann::json::array();
  nlohmann::json fourway = nlohmann::json::array();
  nlohmann::json docks = nlohmann::json::array();
  nlohmann::json center_yield = nlohmann::json::array();

  commander.ForEachJunction([&](int id, Junction& junc) {
    TrafficNode* node = junc.GetEntity();

    if (Factory* typed = dynamic_cast<Factory*>(node)) {
      factories.push_back(*typed);
      for(Dock* d: typed->GetDocks()) {
        docks.push_back(*d);
      }
    } 
    else if (auto* typed = dynamic_cast<FourWayStop*>(node)) {
      fourway.push_back(*typed);
    } else if (auto* typed = dynamic_cast<CenterYield*>(node)) {
      center_yield.push_back(*typed);
    } else {
      TraceLog(LOG_ERROR,"TrafficNode of underterminable type");
    }

    junctions.push_back(junc);
  });

  save["junctions"] = junctions;
  save["factories"] = factories;
  save["fourway"] = fourway;
  save["docks"] = docks;
  save["centerYields"] = center_yield;
  
  std::ofstream stream("pretty.json");
  stream << std::setw(2) << save << std::endl;

}
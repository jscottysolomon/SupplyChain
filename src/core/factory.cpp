#include "factory.hpp"

#include <raylib.h>
#include <raymath.h>
#include <unordered_map>

#include "factory.hpp"
#include "intersection.hpp"
#include "road.hpp"
#include "truck.hpp"
#include "util.hpp"
#include "widget.hpp"

void Factory::OnTick() {
  Produce();
  DockOnTick();
}

void Factory::DockOnTick() {

}

void Factory::Produce() {
  if (organizer == nullptr) return;

  for (ProductionLine& line: production_lines_) {
    if(line.id < 0) { continue; }

    if (line.last_production < 0) {
      line.last_production = line.production_start = GetGlobalTime();
    } else if (organizer->ProduceWidget(line.id,&inventory_, 
        line.production_start, line.last_production)) {
          line.last_production = GetGlobalTime();
          line.production_start = GetGlobalTime();
    }
  }
}

Dock* Factory::DockRequest(Truck* truck) {
  for (Dock* d: docks_) {
    if (!d->assigned) {
      d->assigned = true;
      d->truck = truck;
      return d;
    }
  }

  return nullptr;
}

void Factory::Undock(Truck* truck) {
  for (Dock* dock: docks_) {
    if (dock->truck == truck) {
      dock->assigned = false;
      dock->cargo_ready = false;
      dock->truck = nullptr;
    }
  }
}

FactoryBuilder& FactoryBuilder::WithRoad(Road* road) {
  factory->SetRoad(road);

  for (Intersection* inter: road->GetIntersections()) {
    if (inter->HasRoad(road)) {
      if (factory->GetIntersection() == nullptr) {
        factory->SetIntersection(inter);
      } else if (Vector2Distance(factory->GetPosition(), inter->GetPosition())
        < Vector2Distance(factory->GetIntersection()->GetPosition(),factory->GetPosition())) {
          factory->SetIntersection(inter);
        }
    }

  }

  return *this;
}
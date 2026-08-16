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
  for (ProductionLine line: production_lines_) {
    if ((GetGlobalTime() - line .last_production)  / CLOCKS_PER_SEC < organizer->GetProductionTime(line.id)) {
      return;
    }
    if (organizer->ProduceWidget(&inventory_,line.id)) {
      line.last_production = GetGlobalTime();
      return;
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
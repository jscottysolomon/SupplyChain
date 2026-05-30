#include "factory.hpp"

#include <raylib.h>
#include <raymath.h>
#include <unordered_map>

#include "factory.hpp"
#include "intersection.hpp"
#include "road.hpp"
#include "widget.hpp"

void Factory::OnTick() {
    //Create Widget & Consume Resources
    //Load/Unload docked trucks
}

void Factory::UnloadRequest(Truck* truck, LoadPlan* plan) {
    if(plan == nullptr) return;
    
    plan->UnloaderAgreed();
}

// LoadPlan Factory::LoadRequest(Truck* truck, LoadPlan plan) {

//     return plan;
// }

Dock* Factory::DockRequest(Truck* truck) {
    for(Dock* d: docks_) {
        if(!d->assigned) {
            d->assigned = true;
            d->truck = truck;
            return d;
        }
    }

    return nullptr;
}

FactoryBuilder& FactoryBuilder::WithRoad(Road* road) {
    factory->SetRoad(road);

    for(Intersection* inter: road->GetIntersections()) {
        if(inter->HasRoad(road)) {
            if(factory->GetIntersection() == nullptr) {
                factory->SetIntersection(inter);
            } else if(Vector2Distance(factory->GetPosition(), inter->GetPosition())
                < Vector2Distance(factory->GetIntersection()->GetPosition(),factory->GetPosition())) {
                    factory->SetIntersection(inter);
                }
        }

    }

    return *this;
}
#include "factory.hpp"

#include <raylib.h>
#include <raymath.h>

#include "factory.hpp"
#include "intersection.hpp"
#include "road.hpp"

void Factory::OnTick() {

}

Dock* Factory::DockRequest(Truck* truck) {
    for(Dock* d: docks_) {
        if(!d->occupied) {
            d->occupied = true;
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
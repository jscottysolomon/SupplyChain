#include "factory.hpp"

#include <raylib.h>
#include <raymath.h>
#include <unordered_map>

#include "factory.hpp"
#include "intersection.hpp"
#include "load_plan.hpp"
#include "road.hpp"
#include "truck.hpp"
#include "widget.hpp"

void Factory::OnTick() {
    Produce();
    DockOnTick();
}



/**/
void Factory::DispatchRequest(Truck* truck, LoadPlan* plan) {
    if(plan == nullptr || truck == nullptr) return;

    for(Dock* d: docks_) {
        if(d->truck == truck) {
            d->dispatch_plan = plan;
        }
    }

    std::vector<int> to_remove;

    for(std::pair<int,WidgetStrategy*> p: *plan->GetWidgets()) {
        if(inventory_.GetWidgetQuantity(p.first) <= 0) {
            to_remove.push_back(p.first);
        } else if (inventory_.GetWidgetQuantity(p.first) < p.second->GetAmount()) {
            p.second->SetAmount(inventory_.GetWidgetQuantity(p.first));
        }
    }

    for(int id: to_remove) {
        plan->RemoveWidgetPlan(id);
    }
    
    plan->UnloaderAgreed();
}

void Factory::RequestReceiving(Dock* d) {
    d->receiving_plan = new LoadPlan();
    d->receiving_plan->SetReceiverInventory(&inventory_);
    d->truck->SetUnloaderInventory(d->receiving_plan);

    std::set<int> lst = GetRecipeIds();

    d->receiving_plan->SetWhitelist(lst);

    for(int id: lst) {
        ExactQuantityStrategy* strat = new ExactQuantityStrategy(20);
        d->receiving_plan->AddWidgetStrategy(strat,id);
    }
    
    PrimaryStrategy* finish = new PrimaryStrategy();
    d->receiving_plan->AddFinisherStrategy(finish);

    d->truck->ReceivingRequest(d->receiving_plan,this);
}

void Factory::DockOnTick() {
    for(Dock* d: docks_) {
        if(d->cargo_ready && d->receiving_plan == nullptr && d->truck != nullptr) {
            RequestReceiving(d);
        } else if(d->receiving_plan != nullptr) {
            d->receiving_plan->Load();
        }
    }
}

void Factory::Produce() {
    if(organizer == nullptr) return;
    for(ProductionLine line: production_lines_) {
        if((GetGlobalTime() - line .last_production)  / CLOCKS_PER_SEC < organizer->GetProductionTime(line.id)) {
            return;
        }
        if(organizer->ProduceWidget(&inventory_,line.id)) {
            line.last_production = GetGlobalTime();
            return;
        }
    }
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

void Factory::Undock(Truck* truck) {
    for(Dock* dock: docks_) {
        if(dock->truck == truck) {
            dock->dispatch_plan = nullptr;
            if(dock->receiving_plan != nullptr) {
                delete dock->receiving_plan;
                dock->receiving_plan = nullptr;
            }
            dock->assigned = false;
            dock->cargo_ready = false;
            dock->truck = nullptr;
        }
    }
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
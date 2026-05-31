#include "truck.hpp"

#include <algorithm>
#include <raylib.h>
#include <raymath.h>
#include <queue>

#include "factory.hpp"
#include "intersection.hpp"
#include "inventory.hpp"
#include "road.hpp"
#include "widget.hpp"

void Truck::OnTick() {
    Move();
    Load();
}

void Truck::OnDock() {

}

void Truck::RequestUnload() {
    if(stops_.empty()) return;

    
}

void Truck::Unload() {

}

void Truck::RequestLoad() {
    if(stops_.empty()) return;

    if(load_plan_ != nullptr) {
        delete load_plan_;
    }

    load_plan_ = new LoadPlan;
    std::set<int> whitelist = {1,2,3};

    inventory_.SetWhitelist(whitelist);

    load_plan_->SetReceiverInventory(&inventory_);
    stops_.at(0)->SetPlanInventory(load_plan_,false);
    load_plan_->SetWhitelist(whitelist);

    ExactQuantityStrategy* strat = new ExactQuantityStrategy(50);
    load_plan_->AddWidgetStrategy(strat,1);

    MinimumQuantityStrategy* strat1 = new MinimumQuantityStrategy(20);
    load_plan_->AddWidgetStrategy(strat1,2);   

    // ToCapacityStrategy* finish = new ToCapacityStrategy(390);
    PrimaryStrategy* finish = new PrimaryStrategy();
    load_plan_->AddFinisherStrategy(finish);

    LoadListStrategy* list = new LoadListStrategy();
    load_plan_->AddTertieryStrategy(list);

    stops_.at(0)->UnloadRequest(this,load_plan_);
}

void Truck::Load() {
    if(load_plan_ == nullptr) return;
    if(!docked_) return;

    load_plan_->Load();

    if(load_plan_->IsFinished()) {
        stops_.erase(stops_.begin());
        delete load_plan_;
        load_plan_ = nullptr;
        docked_ = false;
        dock_ = nullptr;
        create_route = true;
    }
}

//TODO implement:
//1. full capacity, x number of items, have x amt that factory needs, et cetera
void CheckUndock() {
    
}

void Truck::Move() {
    if(stops_.empty()) return;
    if(docked_) return;

    if(create_route && !stops_.empty()) {
        route_ = controller_.RequestRoute(intersection_,stops_.at(0)->GetIntersection());
        create_route = false;
    }

    if(intersection_ == nullptr && dock_ == nullptr) {
        intersection_ = route_.front();
        route_.pop();
        target_ = intersection_->GetPosition();
    }
    
    Vector2 movement_vector = Vector2Subtract(target_, position_);
    Vector2 movement = Vector2Scale(movement_vector, speed_);
    position_ = Vector2Add(position_, movement);

    if(Vector2Distance(position_, target_) <= 5) {
        position_ = target_;
        if(dock_ != nullptr) {
            docked_ = true;
            RequestLoad();
        } else if(route_.size() > 0) {
            intersection_ = nullptr;
        } else if(Vector2Distance(position_,stops_.at(0)->GetPosition()) <= 5) {
            dock_ = controller_.RequestDock(stops_.at(0),this);
            if(dock_ != nullptr) {
                target_ = dock_->position;
            } else {
                //TODO add default behavior if dock is full
                //1. Wait 2. Add to end of list
                stops_.erase(stops_.begin());
                if(!stops_.empty() && intersection_ != nullptr) {
                    route_ = controller_.RequestRoute(intersection_, 
                            stops_.at(0)->GetIntersection());
                }
            }
        } else if(route_.size() == 0) {
            target_ = stops_.at(0)->GetPosition();
        }
    }

}

void Truck::AddStop(std::vector<Factory*> facts) {
    for(Factory* f: facts) {
        AddStop(f);
    }
}

void Truck::AddStop(Factory* factory) {
    if(stops_.size() <= 0) {
        for(Intersection* i: current_road_->GetIntersections()) {
            if(intersection_ == nullptr) {
                intersection_ = i;
            } else if( Vector2Distance(position_,i->GetPosition())
                < Vector2Distance(position_,intersection_->GetPosition())) {
                    intersection_ = i;
            }
        }
        route_ = controller_.RequestRoute(intersection_,factory->GetIntersection());
        intersection_ = nullptr;
    }
    stops_.push_back(factory);
}
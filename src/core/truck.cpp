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
    Drive();
    Receive();
    Dispatch();   
    Stall();
}

void Truck::Stall() {
    if(state_ != kStalling) return;

    // return;
}

void Truck::ReceivingRequest(LoadPlan* plan, Factory* f) {
    if(plan == nullptr || f == nullptr) 
        {return;}
    if(state_ != kReceiving) return;

    receiving_plan_ = plan;

    std::vector<int> to_remove;

    for(std::pair<int,WidgetStrategy*> p: *receiving_plan_->GetWidgets()) {
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

void Truck::Receive() {
    if(receiving_plan_ == nullptr) return;
    if(!dock_) return;
    if(state_ != kReceiving) return;

    if(receiving_plan_->IsFinished() && dispatch_plan_ == nullptr) {
        SetState(kDispatching);
        RequestDispatch(); //Requesting Dispatch after factory receiving
    }
}

void Truck::RequestDispatch() {
    if(stops_.empty()) return;
    if(state_ != kDispatching) return;

    if(dispatch_plan_ != nullptr) {
        delete dispatch_plan_;
    }

    dispatch_plan_ = new LoadPlan;
    dispatch_plan_->SetReceiverInventory(&inventory_);
    stops_.front()->SetPlanInventory(dispatch_plan_,false);

    std::set<int> whitelist;

    for(ProductionLine line: stops_.front()->GetProductionLines()) {
        if(!whitelist.count(line.id)) {
            whitelist.insert(line.id);
            ExactQuantityStrategy* strat = new ExactQuantityStrategy(20);
            dispatch_plan_->AddWidgetStrategy(strat,line.id);
        }
    }

    inventory_.SetWhitelist(whitelist);
    dispatch_plan_->SetWhitelist(whitelist);

    PrimaryStrategy* finish = new PrimaryStrategy();
    dispatch_plan_->AddFinisherStrategy(finish);

    LoadListStrategy* list = new LoadListStrategy();
    dispatch_plan_->AddTertieryStrategy(list);

    stops_.front()->DispatchRequest(this,dispatch_plan_);
}

void Truck::Dispatch() {
    if(dispatch_plan_ == nullptr) return;
    if(!docked_) return;
    if(state_ != kDispatching) return;

    dispatch_plan_->Load();

    if(dispatch_plan_->IsFinished()) {
        stops_.front()->Undock(this);
        stops_.pop();
        delete dispatch_plan_;
        dispatch_plan_ = nullptr;
        receiving_plan_ = nullptr;
        docked_ = false;
        dock_ = nullptr;
        create_route = true;
        SetState(kDriving);
        if(stops_.empty()) {
            SetState(kStalling);
        }
    }
}

void Truck::Drive() {
    if(stops_.empty()) return;
    if(docked_) return;
    if(state_ != kDriving) return;

    if(create_route && !stops_.empty()) {
        route_ = controller_.RequestRoute(intersection_,stops_.front()->GetIntersection());
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
            SetState(kReceiving);
            dock_->cargo_ready = true;
        } else if(route_.size() > 0) {
            intersection_ = nullptr;
        } else if(Vector2Distance(position_,stops_.front()->GetPosition()) <= 5) {
            dock_ = controller_.RequestDock(stops_.front(),this);
            if(dock_ != nullptr) {
                target_ = dock_->position;
            } else {
                //TODO add default behavior if dock is full
                //1. Wait 2. Add to end of list
                stops_.pop();
                if(!stops_.empty() && intersection_ != nullptr) {
                    route_ = controller_.RequestRoute(intersection_, 
                            stops_.front()->GetIntersection());
                }
            }
        } else if(route_.size() == 0) {
            target_ = stops_.front()->GetPosition();
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
    stops_.push(factory);
}
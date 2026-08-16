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

void Truck::Receive() {
  if(!dock_) return;
  if(state_ != kReceiving) return;

  Plan* p = plans_.at((stops_[0]->GetId()));

  if(p != nullptr) {
    p->NextAction();
    if(p->IsDone()) {
      SetState(kDispatching);
    }
  }
}

void Truck::Dispatch() {
//   if(dispatch_plan_ == nullptr) return;
  if(!docked_) return;
  if(state_ != kDispatching) return;

  stops_.front()->Undock(this);
  stops_.erase(stops_.begin());
  docked_ = false;
  dock_ = nullptr;
  create_route = true;
  SetState(kDriving);
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
        //Default behavior is to stall on road and wait for dock
        //to be free.
        //TODO add default behavior if dock is full
        //1. Wait 2. Add to end of list
        // stops_.erase(stops_.begin());
        // if(!stops_.empty() && intersection_ != nullptr) {
        //   route_ = controller_.RequestRoute(intersection_, 
        //       stops_.front()->GetIntersection());
        // }
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
  stops_.push_back(factory);
  schedule_.push_back(factory);

  if(contexts_.find(factory->GetId()) == contexts_.end()) {
    RuleContext context;
    context.factory_inv = factory->GetInventory();
    context.truck_inv = GetInventory();
    contexts_.insert({factory->GetId(),context});

    plans_.insert({factory->GetId(), new Plan(context)});
  }
}
#include "truck.hpp"

#include <algorithm>
#include <raylib.h>
#include <raymath.h>
#include <queue>

#include "common.hpp"
#include "factory.hpp"
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
  if (state_ != kStalling) return;
}

void Truck::Receive() {
  if (!dock_id_) return;
  if (state_ != kReceiving) return;

  Plan* p = plans_.at(dynamic_schedule_ids_.at(0));

  if (p != nullptr) {
    p->NextAction();
    if (p->IsDone()) {
      SetState(kDispatching);
    }
  }
}

void Truck::Dispatch() {
  if (!docked_) return;
  if (state_ != kDispatching) return;

  dynamic_schedule_ids_.erase(dynamic_schedule_ids_.begin());
  docked_ = false;
  dock_id_ = -1;
  create_route = true;
  SetState(kDriving);
}

void Truck::Drive() {
  if (dynamic_schedule_ids_.empty()) return;
  if (docked_) return;
  if (state_ != kDriving) return;

  //1. Set Target if no Target
  if(!HasValidTarget()) {
    if(!DeriveNextTarget()) {
      TraceLog(LOG_ERROR, "No target or junction stops, but trying to drive!");
    }
  } 

  float distance = Vector2Distance(position_, target_);
  //2. Move if not at target
  if(distance > 2.0f) {
    MoveToTarget();
  } else {
    //3. Snap to Target
    SetPosition(target_);
    

    //4. Tell factory we've arrived if factory via mediator
    // Factory *factory = junction_->GetFactory();
    // if(factory && factory.) {

    // }

    //5. Go to next target
    if(!pathway_ids_.empty()) {
      pathway_ids_.pop_front();
      DeriveNextTarget();
    //6 OR Dock at dock via mediator_
    } else {
      return;

    }
  }
}

void Truck::AddStop(std::vector<Junction*> junctions) {
  for (Junction* junc: junctions) {
    AddStop(junc);
  }
}

void Truck::AddStop(Junction* junction) {
  if (dynamic_schedule_ids_.size() <= 0) {
    pathway_ids_ = mediator_->RequestRoute(junction_id_,junction);
  }
  dynamic_schedule_ids_.push_back(junction->GetId());
  fixed_schedule_ids_.push_back(junction->GetId());

  Factory* factory = junction->GetFactory();
  
  if(factory && contexts_.find(factory->GetId()) == contexts_.end()) {
    RuleContext context;
    context.factory_inv = factory->GetInventory();
    context.truck_inv = GetInventory();
    contexts_.insert({factory->GetId(),context});

    plans_.insert({factory->GetId(), new Plan(context)});
  }
}

bool Truck::DeriveNextTarget() {
  if(!pathway_ids_.empty()) {
    junction_id_ = pathway_ids_.front();
    if(Junction* j = mediator_->GetJunction(junction_id_)) {
      target_ = j->GetPosition();
    } else {
      TraceLog(LOG_WARNING,"Searched for non-existant id");
    }
    return true;
  } else {
    // SetTarget({-1,-1});
    // junction_ = nullptr;
    return false;
  }
}
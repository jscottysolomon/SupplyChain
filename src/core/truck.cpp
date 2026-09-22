#include "truck.hpp"

#include <algorithm>
#include <raylib.h>
#include <raymath.h>
#include <queue>

#include "common.hpp"
#include "factory.hpp"
#include "inventory.hpp"
#include "widget.hpp"
#include "traffic.hpp"

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

  // Plan* p = plans_.at(dynamic_schedule_ids_.at(0));

  // if (p != nullptr) {
  //   p->NextAction();
  //   if (p->IsDone()) {
  //     SetState(kDispatching);
  //   }
  // }
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
    //Snap to Target
    SetPosition(target_);

    //Go to next target
    if(!pathway_ids_.empty()) {
      pathway_ids_.pop_front();
      DeriveNextTarget();
    //OR Request specific Dock
    } else if(dock_id_ <= -1) {
      dock_id_ = trafficServicer_->AssignDock(junction_id_,id_);
      //TODO:add new junc to list or whatever
      return;

    //OR Dock at dock
    } else {
      trafficServicer_->DockTruck(id_,junction_id_,dock_id_);
    }
  }
}

void Truck::AddToSchedule(std::vector<Junction*> junctions) {
  for (Junction* junc: junctions) {
    AddToSchedule(junc);
  }
}

void Truck::AddToSchedule(Junction* junction) {
  if (dynamic_schedule_ids_.size() <= 0) {
    pathway_ids_ = trafficServicer_->RequestRoute(junction_id_,junction);
  }
  dynamic_schedule_ids_.push_back(junction->GetId());
  fixed_schedule_ids_.push_back(junction->GetId());

  Factory* factory = junction->GetFactory();
}

bool Truck::DeriveNextTarget() {
  if(!pathway_ids_.empty()) {
    junction_id_ = pathway_ids_.front();
    if(Junction* j = trafficServicer_->GetJunction(junction_id_)) {
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
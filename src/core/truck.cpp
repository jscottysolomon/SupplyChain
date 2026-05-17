#include "truck.hpp"

#include <algorithm>
#include <raylib.h>
#include <raymath.h>
#include <queue>

#include "factory.hpp"
#include "intersection.hpp"
#include "road.hpp"

void Truck::OnTick() {
    Move();
    OnDock();
}

void Truck::OnDock() {

}

void Truck::Move() {
    if(stops_.empty()) return;
    if(stops_.empty()) return;
    if(docked_) return;

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
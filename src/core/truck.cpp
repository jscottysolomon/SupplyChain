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
}

void Truck::Move() {
    if(stops_.empty()) return;
    if(stops_.empty()) return;
    if(docked_) return;

    if(intersection_ == nullptr) {
        intersection_ = route_.front();
        route_.pop();
        target_ = intersection_->GetPosition();
    }
    
    Vector2 movement_vector = Vector2Subtract(target_, position_);
    Vector2 movement = Vector2Scale(movement_vector, speed_);
    position_ = Vector2Add(position_, movement);

    if(Vector2Distance(position_, target_) <= 5) {
        position_ = target_;
        if(route_.size() > 0) {
            intersection_ = nullptr;
        } else if(Vector2Distance(position_,stops_.at(0)->GetPosition()) <= 5) {
            stops_.erase(stops_.begin());
            if(!stops_.empty() && intersection_ != nullptr) {
                route_ = controller_.RequestRoute(intersection_, 
                        stops_.at(0)->GetIntersection());
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
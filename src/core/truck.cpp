#include "truck.hpp"

#include <bits/stdc++.h>
#include <raylib.h>
#include <raymath.h>

#include "factory.hpp"
#include "intersection.hpp"
#include "road.hpp"

void Truck::OnTick() {
    Move();
}

void Truck::Move() {
    if(directions_.size() <=0 ) return;
    if(directions_[0].size() <= 0) return;
    if(stops_.size() <=0) return;
    if(docked_) return;
    if(factory_ != nullptr) return;

    if(intersection_ == nullptr && directions_[0].size() > 1) {
        intersection_ = directions_[0][0]->GetIntersection(directions_[0][1]);
        target_ = intersection_->GetPosition();
    } else if(directions_[0].size() == 1)  {
        target_ = stops_[0]->GetPosition();
    }

    Vector2 movement_vector = Vector2Subtract(target_, position_);

    Vector2 movement = Vector2Scale(movement_vector, speed_);

    position_ = Vector2Add(position_, movement);

    if(Vector2Distance(position_, target_) <= 10) {
        if(directions_[0].size() > 1) {
            directions_[0].erase(directions_[0].begin());
            intersection_ = nullptr;
        } else {
            stops_.erase(stops_.begin());
            directions_.erase(directions_.begin());
            intersection_ = nullptr;
        }
        
    }

}

void Truck::AddStop(Factory* factory) {
    bool add = false;
    std::vector<Road*> dir;

    if(directions_.size() <= 0) {
        dir = CalculateRoute(0,factory,{},current_road_);
    } else {
        std::vector<Road*> prev_route = directions_[directions_.size() - 1];
        dir = CalculateRoute(0,factory,{},prev_route[prev_route.size() - 1]);
    }

    add = dir.size() > 0;
    int i =0;

    if(add) {
        stops_.push_back(factory);
        directions_.push_back(dir);
    }
        
}

//TODO: edge case where two factories are on the same road
std::vector<Road*> Truck::CalculateRoute(int len, Factory* factory, std::vector<Road*> route, Road* road) {
    if(road == nullptr) {
        return {};
    }

    len++;
    bool contains = (count(route.begin(),route.end(),road) > 0);
    if(contains) {
        return {};
    }

    route.push_back(road);

    for(Factory* f: road->GetFactories()) {
        if(f == factory) {
            len++;
            return route;
        }
    }

    for(Road* r : road->GetRoads()) {
        if(r != road) {
            std::vector<Road*> result = CalculateRoute(len, factory, route, r);

            if(!result.empty()) {
                return result;
            }
        }
    }

    return route;
}
#include "truck.hpp"

#include <bits/stdc++.h>
#include <raylib.h>
#include <raymath.h>

#include "factory.hpp"
#include "road.hpp"

void Truck::OnTick() {
    Move();
}

void Truck::Move() {
    if(route_.size() <=0 ) return;
    if(docked_) return;
    if(factory_ == nullptr) return;

    float speed = 5.0f;

    Vector2 movement_vector = Vector2Subtract(factory_->GetPosition(), position_);

    Vector2 movement = Vector2Scale(movement_vector, speed);

    position_ = Vector2Add(position_, movement);

}

void Truck::AddStop(Factory* factory) {
    bool add = false;

    std::vector<Road*> route = CalculateRoute(0,factory,{},current_road_);

    add = route.size() > 0;
    int i =0;

    if(add)
        stops_.push_back(factory);

}

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
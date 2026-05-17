#include "truck.hpp"

#include <algorithm>
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
    // if(directions_[0].size() <= 0) return;
    if(stops_.size() <=0) return;
    if(docked_) return;
    if(factory_ != nullptr) return;

    //Case 3: To Factory

    //CASE 1: Two interconnected intersection
    if(intersection_ == nullptr && directions_[0].size() > 1) {
        intersection_ = directions_[0][0]->GetIntersection(directions_[0][1]);
        target_ = intersection_->GetPosition();
        current_road_ = directions_[0][0];
    //CASE 2: To last intersection
    } else if(intersection_ == nullptr && directions_[0].size() == 1)  {
        intersection_ = directions_[0][0]->GetIntersection(current_road_);
        target_ = intersection_->GetPosition();
        //two cases: 
    } 
    // else if(directions_[0].size() <= 0) {
    //     target_ = stops_[0]->GetPosition();
    // }

    Vector2 movement_vector = Vector2Subtract(target_, position_);

    Vector2 movement = Vector2Scale(movement_vector, speed_);

    position_ = Vector2Add(position_, movement);

    if(Vector2Distance(position_, target_) <= 5) {
        // position_ = target_;
        //Calcualte Next intersection
        if(directions_[0].size() > 1) {
            directions_[0].erase(directions_[0].begin());
            intersection_ = nullptr;
        //Calculate Last Intersection
        } else if(directions_[0].size() == 1) {
            // stops_.erase(stops_.begin());
            // directions_.erase(directions_.begin());
            directions_[0].erase(directions_[0].begin());
            intersection_ = nullptr;
            // target_ = factory_->GetPosition();
        //Go to Factory
        } else if(Vector2Distance(position_, target_ = stops_[0]->GetPosition()) <= 5) {
            stops_.erase(stops_.begin());
            directions_.erase(directions_.begin());
        } else {
            target_ = stops_[0]->GetPosition();
            intersection_ = nullptr;
        }
    }

}

void Truck::AddStop(std::vector<Factory*> facts) {
    for(Factory* f: facts) {
        AddStop(f);
    }
}

void Truck::AddStop(Factory* factory) {
    bool add = false;
    std::vector<Road*> dir;
    std::vector<Intersection*> route;

    if(directions_.size() <= 0) {
        dir = CalculateRoute(0,factory,{},current_road_);
        
        for(Intersection* i: current_road_->GetIntersections()) {
            if(intersection_ == nullptr) {
                intersection_ = i;
            } else if( Vector2Distance(position_,i->GetPosition())
                < Vector2Distance(position_,intersection_->GetPosition())) {
                    intersection_ = i;
            }
        }

        route = controller_.RequestRoute(intersection_,factory->GetIntersection());


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

std::vector<Road*> Truck::CalculateRoute(Factory* target, Road* start) {
    // if (start == nullptr || target == nullptr) {
    //     return {};
    // }

    // std::queue<Road*> q;
    // std::unordered_map<Road*, Road*> parent;
    // std::unordered_set<Road*> visited;

    // q.push(start);
    // visited.insert(start);
    // parent[start] = nullptr;

    // Road* goalRoad = nullptr;

    // while (!q.empty()) {
    //     Road* road = q.front();
    //     q.pop();

    //     for (Factory* f : road->GetFactories()) {
    //         if (f == target) {
    //             goalRoad = road;
    //             break;
    //         }
    //     }

    //     if (goalRoad != nullptr) {
    //         break;
    //     }

    //     for (Road* next : road->GetRoads()) {
    //         if (next != nullptr && visited.insert(next).second) {
    //             parent[next] = road;
    //             q.push(next);
    //         }
    //     }
    // }

    // if (goalRoad == nullptr) {
    //     return {};
    // }

    // std::vector<Road*> route;
    // for (Road* r = goalRoad; r != nullptr; r = parent[r]) {
    //     route.push_back(r);
    // }

    // std::reverse(route.begin(), route.end());

    // for(Road* r: route) {
    //     // directs_.push(r);
    // }
    // return route;
    return {};
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
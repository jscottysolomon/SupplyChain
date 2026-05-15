#include "truck.hpp"

#include <bits/stdc++.h>
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
    if (docked_ || factory_ != nullptr) return;
    if (stops_.empty()) return;
    if (current_road_ == nullptr) return;

    if (intersection_ == nullptr) {
        if (!directions_.empty()) {
            Road* nextRoad = directions_.front();
            intersection_ = current_road_->GetIntersection(nextRoad);
            if (intersection_ == nullptr) return;
            target_ = intersection_->GetPosition();
        } else {
            target_ = stops_.front()->GetPosition();
        }
    }

    Vector2 delta = Vector2Subtract(target_, position_);
    float dist = Vector2Length(delta);
    float step = speed_ * GetFrameTime();

    if (dist <= step) {
        position_ = target_;

        if (intersection_ != nullptr) {
            current_road_ = directions_.front();
            directions_.pop();
            intersection_ = nullptr;

            if (!directions_.empty()) {
                Road* nextRoad = directions_.front();
                intersection_ = current_road_->GetIntersection(nextRoad);
                if (intersection_ == nullptr) return;
                target_ = intersection_->GetPosition();
            } else if (!stops_.empty()) {
                target_ = stops_.front()->GetPosition();
            }
        } else {
            // reached factory
            stops_.erase(stops_.begin());
            if (!stops_.empty()) {
                directions_ = CalculateRoute(stops_.front(), current_road_);
            }
        }
    } else {
        position_ = Vector2Add(position_, Vector2Scale(Vector2Normalize(delta), step));
    }
}

// void Truck::Move() {
//     if(directions_.size() <=0 ) return;
//     // if(directions_[0].size() <= 0) return;
//     if(stops_.size() <=0) return;
//     if(docked_) return;
//     if(factory_ != nullptr) return;

//     //Case 3: To Factory

//     //CASE 1: Two interconnected intersection
//     if(intersection_ == nullptr && directions_.size() > 1) {
//         // current_road_ = directions_.front();
//         directions_.pop();
//         intersection_ = directions_.front()->GetIntersection(current_road_);
//         target_ = intersection_->GetPosition();
//         // current_road_ = directions_.front();
//     //CASE 2: To last intersection
//     } else if(intersection_ == nullptr && directions_.size() == 1)  {
//         intersection_ = directions_.front()->GetIntersection(current_road_);
//         target_ = intersection_->GetPosition();
//         //two cases: 
//     } 
//     // else if(directions_[0].size() <= 0) {
//     //     target_ = stops_[0]->GetPosition();
//     // }

//     Vector2 movement_vector = Vector2Subtract(target_, position_);

//     Vector2 movement = Vector2Scale(movement_vector, speed_);

//     position_ = Vector2Add(position_, movement);

//     if(Vector2Distance(position_, target_) <= 5) {
//         // position_ = target_;
//         //Calcualte Next intersection
//         if(directions_.size() > 1) {
//             // directions_[0].erase(directions_[0].begin());
//             directions_.pop();
//             intersection_ = nullptr;
//             current_road_ = directions_.front();
//         //Calculate Last Intersection
//         } else if(directions_.size() == 1) {
//             // stops_.erase(stops_.begin());
//             // directions_.erase(directions_.begin());
//             directions_.pop();
//             current_road_ = directions_.front();
//             intersection_ = nullptr;
//             // target_ = factory_->GetPosition();
//         //Go to Factory
//         } else if(Vector2Distance(position_, target_ = stops_[0]->GetPosition()) <= 5) {
//             stops_.erase(stops_.begin());
//             // directions_.erase(directions_.begin());
//             directions_.pop();
//             current_road_ = directions_.front();
//         } else {
//             target_ = stops_[0]->GetPosition();
//             intersection_ = nullptr;
//             if(stops_.size() > 0) {
//                 CalculateRoute(stops_[0],current_road_);
//             }
//         }
//     }

// }

void Truck::AddStop(std::vector<Factory*> facts) {
    for(Factory* f: facts) {
        AddStop(f);
    }
}

void Truck::AddStop(Factory* factory) {
    if(directions_.size() <= 0) {
        directions_ = CalculateRoute(factory,current_road_);
    }

    stops_.push_back(factory);
}

std::queue<Road*> Truck::CalculateRoute(Factory* target, Road* start) {
    if (start == nullptr || target == nullptr) {
        return {};
    }

    std::queue<Road*> q;
    std::unordered_map<Road*, Road*> parent;
    std::unordered_set<Road*> visited;

    q.push(start);
    visited.insert(start);
    parent[start] = nullptr;

    Road* goalRoad = nullptr;

    while (!q.empty()) {
        Road* road = q.front();
        q.pop();

        for (Factory* f : road->GetFactories()) {
            if (f == target) {
                goalRoad = road;
                break;
            }
        }

        if (goalRoad != nullptr) {
            break;
        }

        for (Road* next : road->GetRoads()) {
            if (next != nullptr && visited.insert(next).second) {
                parent[next] = road;
                q.push(next);
            }
        }
    }

    if (goalRoad == nullptr) {
        return {};
    }

    std::vector<Road*> routeVec;
    for (Road* r = goalRoad; r != nullptr; r = parent[r]) {
        routeVec.push_back(r);
    }

    std::reverse(routeVec.begin(), routeVec.end());

    std::queue<Road*> route;
    for (Road* r : routeVec) {
        route.push(r);
    }

    // std::reverse(route.begin(), route.end());
    return route;
}

//TODO: edge case where two factories are on the same road
// std::vector<Road*> Truck::CalculateRoute(int len, Factory* factory, std::vector<Road*> route, Road* road) {
//     if(road == nullptr) {
//         return {};
//     }

//     len++;
//     bool contains = (count(route.begin(),route.end(),road) > 0);
//     if(contains) {
//         return {};
//     }

//     route.push_back(road);

//     for(Factory* f: road->GetFactories()) {
//         if(f == factory) {
//             len++;
//             return route;
//         }
//     }

//     for(Road* r : road->GetRoads()) {
//         if(r != road) {
//             std::vector<Road*> result = CalculateRoute(len, factory, route, r);

//             if(!result.empty()) {
//                 return result;
//             }
//         }
//     }

//     return route;
// }
#include "truck.hpp"

#include <bits/stdc++.h>
#include <raylib.h>

#include "factory.hpp"
#include "road.hpp"

void Truck::AddStop(Factory* factory) {
    bool add = false;

    std::vector<Road*> route = CalculateRoute(0,factory,{},nullptr);

    add = route.size() > 0;

    if(add)
        stops_.push_back(factory);
}

std::vector<Road*> Truck::CalculateRoute(int len, Factory* factory, std::vector<Road*> route, Road* road) {
    if(road == nullptr) {
        return {};
    }

    len++;
    bool contains = (count(route.begin(),route.end(),road) > 0);
    if(contains)    return {};

    route.push_back(road);

    for(Factory* f: road->GetFactories()) {
        if(f == factory) {
            len++;
            return route;
        }
    }

    for(Road* r: road->GetRoads()) {

        if(r != road) {
            return CalculateRoute(len,factory,route,r);
        }
    }

    return route;
}
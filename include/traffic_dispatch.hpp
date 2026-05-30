/**
 * @file traffic_dispatch.hpp
 * @brief 
 * @author J. Scotty Solomon
 * @date 11-Nov-25
 */

#ifndef TRAFFIC_DISPATCH_HPP
#define TRAFFIC_DISPATCH_HPP

#include <raylib.h>
#include <vector>

#include "road.hpp"
class Truck;
class Factory;
class Intersection;
class TrafficControl;
//CANNOT depend on TRAFFIC_DISPATCH_HPP

class TrafficDispatch {
  public:
    std::vector<Intersection*> GetIntersections() {
      return intersections_;
    }
    void AddIntersection(Intersection* inter) {
      intersections_.push_back(inter);
    }
    TrafficDispatch() {
      SetUp();
    }
    ~TrafficDispatch();

    void OnTick();

    void Draw();
  private:
    void SetUp();
    std::vector<Intersection*> intersections_;
    std::vector<Road*> roads_;
    std::vector<Factory*> factories_;
    std::vector<Truck*> trucks_;
    TrafficControl* controller_;
};

#endif
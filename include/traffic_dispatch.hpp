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
//CANNOT depend on TRAFFIC_DISPATCH_HPP

class TrafficDispatch {
  private:
    void SetUp();
    std::vector<Intersection*> intersections_;
    std::vector<Road*> roads_;
    std::vector<Factory*> factories_;
    std::vector<Truck*> trucks_;
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
    ~TrafficDispatch() {
      for(int i = 0; i < roads_.size(); i++) {
        delete roads_[i];
      }
      for(int i = 0; i < factories_.size(); i++) {
        delete factories_[i];
      }
      for(Truck* t:trucks_) {
        delete t;
      }
      for(Intersection* inter: intersections_) {
        delete inter;
      }
    }

    void TrafficHelper(Factory* factory, std::vector<Road*> roads, int length);
    void OnTick();

    void Draw();

};

#endif
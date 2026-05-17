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
  private:
    void SetUp();
    std::vector<Intersection*> intersections_;
    std::vector<Road*> roads_;
    std::vector<Factory*> factories_;
    std::vector<Truck*> trucks_;
    TrafficControl* controller_;
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
      for(std::size_t ii = 0; ii < roads_.size(); ii++) {
        delete roads_.at(ii);
      }
      for(std::size_t i = 0; i < factories_.size(); i++) {
        delete factories_.at(i);
      }
      for(Truck* t:trucks_) {
        delete t;
      }
      for(Intersection* inter: intersections_) {
        delete inter;
      }

      delete controller_;
    }

    void TrafficHelper(Factory* factory, std::vector<Road*> roads, int length);
    void OnTick();

    void Draw();

};

#endif
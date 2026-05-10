/**
 * @file traffice_manager.hpp
 * @brief 
 * @author J. Scotty Solomon
 * @date 11-Nov-25
 */

#ifndef TRAFFIC_MANAGER_HPP
#define TRAFFIC_MANAGER_HPP

#include <raylib.h>
#include <vector>

#include "road.hpp"
class Truck;
class Factory;
//CANNOT depend on traffic_manager_hpp

class TrafficManager {
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
    TrafficManager() {
      SetUp();
    }
    ~TrafficManager() {
      for(int i = 0; i < roads_.size() - 1; i++) {
        delete roads_[i];
      }
      for(int i = 0; i < factories_.size() - 1; i++) {
        delete factories_[i];
      }
      for(Truck* t:trucks_) {
        delete t;
      }
    }

    void TrafficHelper(Factory* factory, std::vector<Road*> roads, int length);

    void Draw();

};

#endif
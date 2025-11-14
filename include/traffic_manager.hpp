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

class TrafficManager {
  private:
    void SetUpIntersections();
    std::vector<Intersection*> intersections_;
    std::vector<Road*> roads_;
  public:
    std::vector<Intersection*> GetIntersections() {
      return intersections_;
    }
    void AddIntersection(Intersection* inter) {
      intersections_.push_back(inter);
    }
    TrafficManager() {
      SetUpIntersections();
    }

    void Draw();

};

#endif
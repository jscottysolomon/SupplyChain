/**
 * @file road.hpp
 * @brief 
 * @author J. Scotty Solomon
 * @date 10-Nov-25
 */

#ifndef ROAD_HPP
#define ROAD_HPP

#include <vector>

#include<raylib.h>

class Factory;

struct Intersection {
  Vector2 position;
  std::vector<Road*> roads;
};

class Road {
  private:
    std::vector<Vector2> points_;
    std::vector<Intersection*> intersections_;
    std::vector<Factory*> factories_;
  public:
    Road(std::vector<Vector2> points) {
      this->points_ = points;
    }
    void AddIntersection(Intersection* intersection) {
      intersections_.push_back(intersection);
    }
    std::vector<Intersection*> GetIntersections() {
      return intersections_;
    }
};

#endif
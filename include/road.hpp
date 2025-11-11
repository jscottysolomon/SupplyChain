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
  Road* road;
};

class Road {
  private:
    std::vector<Vector2> points;
    std::vector<Intersection> intersections;
    std::vector<Factory*> factories;
  public:
};

#endif
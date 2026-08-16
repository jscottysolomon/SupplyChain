#include "road.hpp"

#include <vector>

#include "intersection.hpp"

Intersection* Road::GetIntersection(Road* r2) {
  for (Intersection* inter: intersections_) {
    std::vector<Road*> roads = inter->GetRoads();
    
    for (Road* r: roads) {
      if (r == r2) {
        return inter;
      }
    }
  }

  return nullptr;
}
/**
 * @file road.hpp
 * @brief 
 * @author J. Scotty Solomon
 * @date 10-Nov-25
 */

#ifndef ROAD_HPP
#define ROAD_HPP

#include <vector>

#include <raylib.h>

#include "traffic_node.hpp"

class Factory;

class Road;

struct Intersection {
  Vector2 position;
  std::vector<Road*> roads;
};

class Road : public TrafficNode {
  private:
    std::vector<Vector2> points_;
    std::vector<Intersection*> intersections_;
    std::vector<Factory*> factories_;
  public:
    Road(std::vector<Vector2> points) {
      this->points_ = points;
    }
    void AddIntersection(Intersection* intersection) {
      if(std::count(intersections_.begin(), intersections_.end(), intersection) > 0) {
        intersections_.push_back(intersection);
      }
    }
    std::vector<Intersection*> GetIntersections() {
      return intersections_;
    }
    std::vector<Vector2> GetPoints() {
      return points_;
    }
    void AddFactory(Factory* factory) {
      factories_.push_back(factory);
    }

    Vector2 GetStart() {return points_[0];}
    Vector2 GetEnd() {return points_[1];}
    
    void Draw() {
      // for(int xx = 0; xx < points_.size() - 2; xx++) {
      //   for(int yy = 0; yy < points_.size() - 1; yy++) {
      //     DrawLine(points_[xx].x,points_[xx].y,points_[yy].x,points_[yy].y,WHITE);
      //   }
      // }
      DrawLine(points_[0].x,points_[0].y,points_[1].x,points_[1].y,WHITE);
    }
};

#endif
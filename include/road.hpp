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

class Factory;
class Intersection;

class Road {
  public:
  Road(std::vector<Vector2> points) {
    this->points_ = points;
  }

  std::vector<Road*> GetRoads() {
    return connections_;
  }
  
  void AddRoad(Road* node) {
    connections_.push_back(node);
  }

  void RemoveRoad(Road* node) {
    // connections_.erase(std::remove(connections_.begin(), connections_.end(), node), connections_.end());
  }

  void AddIntersection(Intersection* intersection) {
    intersections_.push_back(intersection);
  }

  Intersection* GetIntersection(Road* r2);

  std::vector<Intersection*> GetIntersections() {
    return intersections_;
  }
  std::vector<Vector2> GetPoints() {
    return points_;
  }
  void AddFactory(Factory* factory) {
    factories_.push_back(factory);
  }

  bool IsHorizontal() {return(points_[0].x == points_[1].x);}

  std::vector<Factory*> GetFactories() {return factories_;}

  Vector2 GetStart() {return points_[0];}
  Vector2 GetEnd() {return points_[1];}
  
  void Draw() {
    DrawLine(points_[0].x,points_[0].y,points_[1].x,points_[1].y,WHITE);
  }
  private:
  std::vector<Vector2> points_;
  std::vector<Intersection*> intersections_;
  std::vector<Factory*> factories_;
  std::vector<Road*> connections_;
};

#endif
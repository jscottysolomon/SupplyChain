#ifndef TRAFFIC_HPP
#define TRAFFIC_HPP

#include <cmath>
#include <queue>
#include <vector>

#include <graaflib/graph.h>
#include <graaflib/edge.h>
#include <raylib.h>

#include "common.hpp"
#include "entity.hpp"
#include "util.hpp"
#include "road.hpp"

class RoadSegment;
class Junction;
class Truck;
class Factory;
class Intersection;
struct Dock;
class TrafficMediator;


/*Map Object that has a "cost" for graph purposes*/
class TrafficNode: public MapObject {
  public:
    TrafficNode(Vector2 pos) : MapObject(pos) {

    }
    void SetJunctionId(int id) 
			{junction_id_ = id;}
		int GetJunctionId() 
			{return junction_id_;}
    int GetCost();
  protected:
    int junction_id_;
};

enum class JunctionType {
  Factory,
  Dock,
  FourWayStop,
  TrafficLight,
  Roundabout,
  CenterYield
};

class Junction : public MapObject{
public:
  Junction(Vector2 position, JunctionType type, TrafficNode* obj, 
      std::vector<RoadSegment*> segments) : MapObject(position) {
    type_ = type;
    obj_ = obj;
    segments_ = segments;
  }

  Junction(TrafficNode* obj, JunctionType type){
    obj_ = obj;
    type_ = type;
    if(obj != nullptr) SetPosition(obj_->GetPosition());
  }

  ~Junction(){
    delete obj_;
    obj_ = nullptr;
  }

  void Draw() override {
    obj_->Draw();
  }
  void OnTick() override {
    obj_->OnTick();
  }

  void AddSegmnet(RoadSegment* seg)
    { segments_.push_back(seg); }
  std::vector<RoadSegment*> GetSegments() 
    { return segments_; }
  bool IsType(JunctionType type) 
    {return type == type_; }
  JunctionType GetType() 
    { return type_; }
  int GetCost() {
    if (obj_ != nullptr) {
      return obj_->GetCost();
    }
    return 0;
  }

  void SetGraphId(graaf::vertex_id_t id)
    {graph_id_ = id;}
  graaf::vertex_id_t GetGraphId() 
    {return graph_id_;}
  
  void RemoveSegment(RoadSegment* rs1) {
    segments_.erase(std::remove(segments_.begin(), segments_.end(), rs1),segments_.end());
  }

private:
  JunctionType type_;
  TrafficNode* obj_;
  std::vector<RoadSegment*> segments_;
  graaf::vertex_id_t graph_id_;
};

class RoadSegment {
public: 
  RoadSegment(std::vector<Vector2> path, Junction* j1, Junction* j2) {
    path_points = path;
    j1_ = j1;   j1_->AddSegmnet(this);
    j2_ = j2;   j2->AddSegmnet(this);
    one_way_ = false;
    lanes_ = 2;
    if(j1 == nullptr || j2 == nullptr) return;

    Vector2 p1 = j1_->GetPosition();
    Vector2 p2 = j2_->GetPosition();

    int offset = 0;

    //TODO: Factor in type of j1 & j2
    switch(j2->GetType()) {
      case JunctionType::Factory:
        offset = FACTORY_WIDTH;
        break;
      case JunctionType::FourWayStop:
        offset = FOUR_WAY_STOP_WIDTH * lanes_;
        break;
      default:
        offset = 0;
    }

    float road_thickness = LANE_WIDTH * lanes_;

    //Vertical
    if (p1.x == p2.x) {
      float top = std::min(p1.y, p2.y) + offset;
      float bottom = std::max(p1.y, p2.y);

      float road_length = bottom - top;
      rectangle_ = {p1.x, top, road_thickness, road_length};

      for (int ii = 1; ii < lanes_; ++ii) {
          float line_x = rectangle_.x + LANE_WIDTH * ii;

          lines_.push_back({{ line_x, rectangle_.y },
              { line_x, rectangle_.y + rectangle_.height }});
      }
    //Horizontal
    } else if (p1.y == p2.y) {
      float left = std::min(p1.x, p2.x);
      float right = std::max(p1.x, p2.x);
      left += offset;

      float road_length = right - left;
      rectangle_ = {left,p1.y,road_length,road_thickness};
      
      for (int ii = 1; ii < lanes_; ++ii) {
        float line_y = rectangle_.y + LANE_WIDTH * ii;

        lines_.push_back({{ rectangle_.x, line_y }, 
          { rectangle_.x + rectangle_.width, line_y}});
      }
    }

  }
  void Draw() {
    DrawRectangleRec(rectangle_,BLACK);
    for (Line2D line: lines_) {
      DrawLineV(line.start,line.end,WHITE);
    }
  }

  void OnTick() {
    
  }

  std::vector<Junction*> GetJunctions() {
    std::vector<Junction*> ret;
    ret.push_back(j1_);
    ret.push_back(j2_);
    return ret;
  }

  void SetGraphId(graaf::vertex_id_t id)
    {graph_id_ = id;}
  graaf::vertex_id_t GetGraphId() 
    {return graph_id_;}
private:
  Junction* j1_;
  Junction* j2_;

  float length_;
  float speed_limit_;
  bool one_way_;      //can only go from j1 to j2
  int id_;
  int lanes_; 

  std::vector<Vector2> path_points;
  std::vector<Line2D> lines_;
  float GetTravelCost() const;
  Rectangle rectangle_;
  graaf::vertex_id_t graph_id_;
};

class FourWayStop : public TrafficNode {
public: 
  FourWayStop(Vector2 pos) : TrafficNode(pos) {

  }

  void Draw() override {
    DrawRectangleV(position_,{(float)FOUR_WAY_STOP_WIDTH*lanes_,
      (float)FOUR_WAY_STOP_WIDTH*lanes_},ORANGE);
  }

  void OnTick() override {
    
  }

private:
  std::vector<Truck*> trucks_;
  int lanes_ = 2;

};

class TrafficGraph {
public:
  std::vector<Junction*> nodes;
  std::vector<RoadSegment*> roads;

  std::vector<RoadSegment*> GetOutgoingRoads(Junction* node);
};

class TrafficCommand {
public:
  std::vector<Intersection*> GetIntersections() {
    return intersections_;
  }
  std::vector<Truck*> GetTrucks() {
    return trucks_;
  }
  std::vector<Factory*> GetFactories() {
    return factories_;
  }

  void AddIntersection(Intersection* inter) {
    intersections_.push_back(inter);
  }
  TrafficCommand() {
    SetUp();
  }
  ~TrafficCommand();

  void OnTick();

  void Draw();
private:
  void SetUp();
  Junction* AddFourWayJunction(Vector2 position);
  Junction* AddFactoryJunction(Factory* factory);
  RoadSegment* AddRoadSegment(Junction* a, Junction* b);
  RoadSegment* AddJunction(RoadSegment* rs1, RoadSegment* rs2, float ratio, JunctionType type);
  Junction* AddJunction(Junction* j1, Junction* j2, JunctionType type, float ratio);
  void ConnectJunctions(Junction* j1, Junction* j2, RoadSegment* rs);
  void RemoveRoad(RoadSegment* rs1, Junction* j1, Junction* j2);
  void RemoveJunction(Junction* junction);

  std::vector<Intersection*> intersections_;
  std::vector<Road*> roads_;
  std::vector<Factory*> factories_;
  std::vector<Truck*> trucks_;
  std::vector<Junction*> junctions_;
  std::vector<RoadSegment*> segments_;
  graaf::directed_graph<Junction*, RoadSegment*> graph_;
  // std::unordered_map<int, graaf::vertex_id_t> vertecies_;
  TrafficMediator* mediator_;
};

struct Vertex{
  Intersection* inter;
  float dist;
  Vertex* prev;
};

class TrafficMediator {
  public:
    TrafficMediator(
      std::vector<Intersection*>& intersections,
      std::vector<Road*>& roads,
      std::vector<Factory*>& factories,
      std::vector<Truck*>& trucks,
      std::vector<Junction*>& junctions,
      std::vector<RoadSegment*>& segments,
      graaf::directed_graph<Junction*, RoadSegment*>& graph
      ): intersections_(intersections), roads_(roads), factories_(factories), trucks_(trucks), 
        junctions_(junctions), segments_(segments), graph_(graph)  {

    }
    std::queue<Intersection*> RequestRoute(Intersection* src, Intersection* dest);
    std::vector<Junction*> RequestRoute(Junction* src, Junction* dest);
    bool RequestIntersection(Intersection* inter, Truck* truck);
    Dock* RequestDock(Factory* factory, Truck* truck);
  private:
    std::vector<Vertex*> Dijkstra(Intersection* src);
    std::vector<Intersection*>& intersections_;
    std::vector<Road*>& roads_;
    std::vector<Factory*>& factories_;
    std::vector<Truck*>& trucks_; 
    std::vector<Junction*>& junctions_;
    std::vector<RoadSegment*>& segments_;
    graaf::directed_graph<Junction*, RoadSegment*>& graph_;
    // std::unordered_map<int, graaf::vertex_id_t>& vertecies_;
};

RoadSegment* GetCommonRoad(Junction* j1, Junction* j2);


#endif
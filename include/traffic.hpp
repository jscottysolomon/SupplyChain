#ifndef TRAFFIC_HPP
#define TRAFFIC_HPP

#include <cmath>
#include <list>
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
      // NextId();
    }
    void SetJunctionId(graaf::vertex_id_t id) 
			{junction_id_ = id;}
		graaf::vertex_id_t GetJunctionId() 
			{return junction_id_;}
    int GetCost();
    int GetLaneNumber() 
      {return lanes_;}
    void SetLaneNumber(int lanes) 
      {lanes_ = lanes;}
  protected:
    graaf::vertex_id_t junction_id_;
    std::vector<Truck*> trucks_;
    int lanes_ = 2;
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
  TrafficNode* GetEntity() 
    { return obj_;}
  Factory* GetFactory();

  void SetGraphId(graaf::vertex_id_t id) {
    graph_id_ = id;
    obj_->SetJunctionId(graph_id_);
  }
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
    CalculateDrawingRectangle();
  }
  void Draw() {
    DrawRectangleRec(rectangle_,BLACK);
    for (Line2D line: lines_) {
      DrawLineV(line.start,line.end,WHITE);
    }
  }

  void SetJunctions(Junction* j1, Junction* j2) {
    j1_ = j1;
    j2_ = j2;
    CalculateDrawingRectangle();
  }

  std::vector<Truck*> GetTrucks() 
    { return trucks_; }
  void AddTruck(Truck* t) 
    { trucks_.push_back(t); }

  void OnTick() {
    
  }
  std::vector<Junction*> GetJunctions() {
    std::vector<Junction*> ret;
    ret.push_back(j1_);
    ret.push_back(j2_);
    return ret;
  }
  Junction* GetStart() 
    { return j1_; }
  Junction* GetEnd() 
    { return j2_; }
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
  std::vector<Truck*> trucks_;
  Rectangle rectangle_;
  graaf::vertex_id_t graph_id_;

  float GetTravelCost() const;
  void CalculateDrawingRectangle();
};

class CenterYield : public TrafficNode {
public:
  CenterYield(Vector2 pos) : TrafficNode(pos) {
    lanes_ = 2;
  }

  void Draw() override {
    DrawRectangleV(position_,{(float)FOUR_WAY_STOP_WIDTH*lanes_,
      (float)FOUR_WAY_STOP_WIDTH*lanes_},YELLOW);
  }

  void OnTick() override {
    
  }
};

class FourWayStop : public TrafficNode {
public: 
  FourWayStop(Vector2 pos) : TrafficNode(pos) {
    lanes_ = 2;
  }

  void Draw() override {
    DrawRectangleV(position_,{(float)FOUR_WAY_STOP_WIDTH*lanes_,
      (float)FOUR_WAY_STOP_WIDTH*lanes_},ORANGE);
  }

  void OnTick() override {
    
  }
};

/**
 * @brief TrafficCommand is in charge of creating any map objects
 * (e.g. Truck, RoadSegment, Junction) and uses the Chain of Command
 * design pattern for OnTick() and Draw()
 * 
 */
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
  void RoadSegmentSetUp();

  /**
   * @brief Create a Mediator object
   * 
   */
  void CreateMediator();

  /**
   * @brief Creates a FourWayStop object and wraps it 
   * in a Junction object.
   * 
   * @param position 
   * @return Junction* 
   */
  Junction* AddFourWayJunction(Vector2 position);

  /**
   * @brief Wraps Factory pointer in a Junction object.
   * 
   * @param factory 
   * @return Junction* 
   */
  Junction* AddFactoryJunction(Factory* factory);

  /**
   * @brief Creates a straight RoadSegment between j1 and 
   * j2 and calls ConnectJunctions(). Addition to segments_ 
   * must be flushed.
   * 
   * @param j1 
   * @param j2 
   * @return RoadSegment* 
   */
  RoadSegment* AddRoadSegment(Junction* j1, Junction* j2);
  
  Junction* AddJunction(Junction* j1, Vector2 pos, JunctionType type);
  
  /**
   * @brief Creates a junction between J1 and J2. Any segments that 
   * need to be added/deleted from segments_ has to be flushed.
   * Connects new roads to new/existing junctions.
   * 
   * @param j1 
   * @param j2 
   * @param type 
   * @param ratio 
   * @return Junction* 
   */
  Junction* AddJunction(Junction* j1, Junction* j2, JunctionType type, float ratio);
  
  /**
   * @brief Creates a junction at the specified position of the specified type.
   * Does not connect any junction
   * 
   * @param type 
   * @param pos 
   * @return Junction* 
   */
  Junction* AddJunction(JunctionType type, Vector2 pos);

  /**
   * @brief Connects junctions to each other via road segment. 
   * Road segment's junction are also set.
   * 
   * @param j1 
   * @param j2 
   * @param rs 
   */
  void ConnectJunctions(Junction* j1, Junction* j2, RoadSegment* rs);

  /**
   * @brief Removes RoadSegment from both junctions and removes 
   * bidirectional edges between junctions. Queues RoadSegment 
   * deletion that must be flushed.
   * 
   * @param rs1 
   * @param j1 
   * @param j2 
   */
  void RemoveRoad(RoadSegment* rs1, Junction* j1, Junction* j2);
  void RemoveJunction(Junction* junction);

  /**
   * @brief Create a Truck object at specified position
   * 
   * @param rs 
   * @param pos 
   * @return Truck* 
   */
  Truck* CreateTruck(RoadSegment* rs, Vector2 pos);

  /**
   * @brief Create a Truck object at ratio of road (e.g. 1/2 way through)
   * 
   * @param segment 
   * @param scale 
   * @return Truck* 
   */
  Truck* CreateTruck(RoadSegment* segment, float scale);

  /**
   * @brief Helper function to flush additions and deletions to segments_.
   * Used to avoid push/removing elements while iterating through funtion
   * 
   */
  void SegmentFlush();

  std::vector<Intersection*> intersections_;
  std::vector<Road*> roads_;
  std::vector<Factory*> factories_;
  std::vector<Truck*> trucks_;
  std::vector<Junction*> junctions_;
  std::vector<RoadSegment*> segments_;
  graaf::directed_graph<Junction*, RoadSegment*> graph_;
  std::queue<RoadSegment*> segment_deletions_;
  std::queue<RoadSegment*> segment_additions_;
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
    std::list<Junction*> RequestRoute(Junction* src, Junction* dest);
    bool RequestIntersection(Intersection* inter, Truck* truck);
    Dock* RequestDock(Factory* factory, Truck* truck);
    Dock* RequestDock(Junction* junction, Truck* truck);
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
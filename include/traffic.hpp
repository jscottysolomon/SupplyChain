#ifndef TRAFFIC_HPP
#define TRAFFIC_HPP

#include <cmath>
#include <functional>
#include <iterator>
#include <list>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>


#include <graaflib/graph.h>
#include <graaflib/edge.h>
#include <nlohmann/json.hpp>
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
class TrafficCommand;


/*Map Object that has a "cost" for graph purposes*/
class TrafficNode: public MapObject {
  public:
    TrafficNode(Vector2 pos) : MapObject(pos) {
      // NextId();
    }
    virtual ~TrafficNode() = default;
    void SetJunctionGraphId(graaf::vertex_id_t id) 
			{junction_graph_id_ = id;}
		graaf::vertex_id_t GetJunctionGraphId() const
			{return junction_graph_id_;}
    void SetJunctionEntityId(int id)
     {junction_entity_id_ = id;}
    int GetJunctionEntityId() const
      {return junction_entity_id_;}
    int GetCost();
    int GetLaneNumber() const
      {return lanes_;}
    std::vector<Truck*> GetTrucks() const
      { return trucks_; }
    void SetLaneNumber(int lanes) 
      {lanes_ = lanes;}
    void OnTick() override = 0;
    void Draw() override = 0;
  protected:
    graaf::vertex_id_t junction_graph_id_;
    int junction_entity_id_;
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

class Junction : public MapObject {
public:
  Junction(Vector2 position, JunctionType type, TrafficNode* obj, 
      std::vector<RoadSegment*> segments) : MapObject(position) {
    type_ = type;
    obj_ = obj;
    segments_ = segments;

    if(obj_) {
      obj_->SetJunctionEntityId(id_);
    }
  }

  Junction(TrafficNode* obj, JunctionType type) : MapObject(obj->GetPosition()){
    obj_ = obj;
    type_ = type;
    if(obj != nullptr) {
      SetPosition(obj_->GetPosition());
      obj_->SetJunctionEntityId(id_);
    }
  }

  ~Junction() override {
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
  int GetEntityId() const {
    if(obj_){
      return obj_->GetId();
    }
    return -1;
  }

  void SetGraphId(graaf::vertex_id_t id) {
    graph_id_ = id;
    obj_->SetJunctionGraphId(graph_id_);
  }
  graaf::vertex_id_t GetGraphId() 
    {return graph_id_;}
  
  void RemoveSegment(RoadSegment* rs1) {
    segments_.erase(std::remove(segments_.begin(), segments_.end(), rs1),segments_.end());
  }

private:
  JunctionType type_;

  /**
   * @brief Junction owns the TrafficNode it wraps.
   * 
   */
  TrafficNode* obj_;
  std::vector<RoadSegment*> segments_;
  graaf::vertex_id_t graph_id_;
};

class RoadSegment {
public: 
  RoadSegment(Junction* j1, Junction* j2) {
    one_way_ = false;
    lanes_ = 2;
    CalculateLeftRight(j1,j2);    
    id_ = NextId();
  }
  void Draw() {
    DrawRectangleRec(rectangle_,BLACK);
    for (Line2D line: lines_) {
      DrawLineV(line.start,line.end,WHITE);
    }
  }

  void SetJunctions(Junction* j1, Junction* j2) {
    CalculateLeftRight(j1,j2);
    CalculateDrawingRectangle();
  }

  std::vector<Truck*> GetTrucks() 
    { return trucks_; }
  void AddTruck(Truck* t) 
    { trucks_.push_back(t); }
  int GetId() 
    { return id_; }

  void OnTick() {
    
  }
  std::vector<Junction*> GetJunctions() {
    std::vector<Junction*> ret;
    ret.push_back(right_);
    ret.push_back(left_);
    return ret;
  }
  /**
   * @brief Junction truck heads to if it's on the right side of the road
   * 
   * @return Junction* 
   */
  Junction* GetRightSideJunction() 
    { return right_; }

  /**
   * @brief Junction truck heads to if it's on the left side of the road
   * 
   * @return Junction* 
   */
  Junction* GetLeftSideJunction()
    { return left_; }

  /**
   * @brief Returns right or left based on param
   * 
   * @param on_right truck is on right side of road
   * @return Junction* 
   */
  Junction* GetJunction(bool on_right) {
    if(on_right) { return right_; }
    return left_;
  }
  void SetGraphId(graaf::vertex_id_t id)
    {graph_id_ = id;}
  graaf::vertex_id_t GetGraphId() 
    {return graph_id_;}
private:
  Junction* right_;
  Junction* left_;

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
  void CalculateLeftRight(Junction* j1, Junction* j2);
};

class CenterYield : public TrafficNode {
public:
  CenterYield(Vector2 pos) : TrafficNode(pos) {
    lanes_ = 2;
  }
  ~CenterYield() override = default;


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
  ~FourWayStop() override = default;

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
  TrafficCommand();
  ~TrafficCommand();

  void OnTick();

  void Draw();

  void ForEachFactory(const std::function<void(int, Factory&)>& fn) {
      for (auto& [id, factory] : factories_) {
          fn(id, *factory);
      }
  }

  Factory* GetFactory(int id) {
      auto it = factories_.find(id);
      if (it == factories_.end()) {
          return nullptr;
      }
      return it->second;
  }

  void RemoveFactory(int id) {
      factories_.erase(id);
  }

  Factory* GetNextFactoryOrFirst(int id) {
    if (factories_.empty()) {
        return nullptr;
    }

    auto it = factories_.find(id);
    if (it == factories_.end()) {
        return factories_.begin()->second;
    }

    auto next = std::next(it);
    if (next == factories_.end()) {
        return factories_.begin()->second;
    }
    return next->second;
  }

  void ForEachTruck(const std::function<void(int, Truck&)>& fn) {
      for (auto& [id, truck]  : trucks_) {
          fn(id, *truck);
      }
  }

  Truck* GetTruck(int id) {
      auto it = trucks_.find(id);
      if (it == trucks_.end()) {
          return nullptr;
      }
      return it->second.get();
  }

  void RemoveTruck(int id);

  Truck* GetNextTruckOrFirst(int id) {
    if (trucks_.empty()) {
        return nullptr;
    }

    auto it = trucks_.find(id);
    if (it == trucks_.end()) {
        return trucks_.begin()->second.get();
    }

    auto next = std::next(it);
    if (next == trucks_.end()) {
        return trucks_.begin()->second.get();
    }
    return next->second.get();
  }

  void ForEachJunction(const std::function<void(int, Junction&)>& fn) {
    for (auto& [id, junction] : junctions_) {
        fn(id, *junction);
    }
  }

  Junction* GetJunction(int id) {
      auto it = junctions_.find(id);
      if (it == junctions_.end()) {
          return nullptr;
      }
      return it->second.get();
  }

  void RemoveJunction(int id) {
      junctions_.erase(id);
  }

  void ForEachSegment(const std::function<void(int, RoadSegment&)>& fn) {
    for (auto& [id, seg] : segments_) {
        fn(id, *seg);
    }
  }

  RoadSegment* GetSegment(int id) {
      auto it = segments_.find(id);
      if (it == segments_.end()) {
          return nullptr;
      }
      return it->second.get();
  }

  void RemoveSegment(int id) {
      segments_.erase(id);
  }

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
  std::unordered_map<int, Factory*> factories_;
  std::unordered_map<int, std::unique_ptr<Truck>> trucks_;
  std::unordered_map<int, std::unique_ptr<Junction>> junctions_;
  std::unordered_map<int, std::unique_ptr<RoadSegment>> segments_;
  graaf::directed_graph<Junction*, RoadSegment*> graph_;
  std::queue<int> segment_deletions_;
  std::queue<std::unique_ptr<RoadSegment>> segment_additions_;
  // std::unordered_map<int, graaf::vertex_id_t> vertecies_;
  TrafficMediator* mediator_;
};

class TrafficMediator {
  public:
    TrafficMediator(TrafficCommand& commander, graaf::directed_graph<Junction*, RoadSegment*>& graph
      ): commander_(commander), graph_(graph)  {

    }
    std::list<int> RequestRoute(int id, Junction* dest);
    bool RequestIntersection(Intersection* inter, Truck* truck);
    Dock* RequestDock(Factory* factory, Truck* truck);
    Dock* RequestDock(Junction* junction, Truck* truck);

    Truck* GetTruck(int id) const
      { return commander_.GetTruck(id); }
    Junction* GetJunction(int id) const
      {return commander_.GetJunction(id);}
    RoadSegment* GetSegment(int id) const
      {return commander_.GetSegment(id);}
      
    
  private:
    TrafficCommand& commander_;
    graaf::directed_graph<Junction*, RoadSegment*>& graph_;
    // std::unordered_map<int, graaf::vertex_id_t>& vertecies_;
};

struct Vertex{
  Intersection* inter;
  float dist;
  Vertex* prev;
};



RoadSegment* GetCommonRoad(Junction* j1, Junction* j2);


#endif
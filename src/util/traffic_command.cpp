/**
 * @file traffice_manager.cpp
 * @brief This is kind of a god object that is going to be reworked to be in charge of
 * creating trucks, factories, et cetera.
 * @author J. Scotty Solomon
 * @date 12-Nov-25
 */

#include "traffic.hpp"

#include <cassert>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <unordered_map>

#include <raylib.h>
#include <raymath.h>
#include <graaflib/algorithm/shortest_path/dijkstra_shortest_path.h>
#include <graaflib/algorithm/shortest_path/bfs_shortest_path.h>

#include "factory.hpp"
#include "intersection.hpp"
#include "util.hpp"
#include "traffic.hpp"
#include "truck.hpp"

#define THRESHOLD 40
#define LENGTH 150
#define Y_START 150
#define X_START 400
#define OFFSET_JUNCTION 50
#define F_OFFSET 100

void TrafficCommand::RoadSegmentSetUp() {
  int length = 150; int num = 3;

  Junction* j1 = AddFourWayJunction({150,150});
  Junction* j2 = AddFourWayJunction({300,150});
  Junction* j3 = AddFourWayJunction({500,150});
  Junction* j8 = AddFourWayJunction({700,150});


  Junction* j4 = AddFourWayJunction({150,300});
  Junction* j5 = AddFourWayJunction({300,300});

  Junction* j6 = AddFourWayJunction({300,450});
  Junction* j7 = AddFourWayJunction({500,450});

  Junction* j9 = AddFourWayJunction({150,700});
  Junction* j10 = AddFourWayJunction({700,700});


  Junction* y1 = AddJunction(JunctionType::CenterYield,{400,450});
  Junction* y2 = AddJunction(JunctionType::CenterYield,{500,275});
  Junction* y3 = AddJunction(JunctionType::CenterYield,{500,375});


  Junction* f1 = AddFactoryJunction(FactoryBuilder({400,550})
    .Capacity(3)
    .WithDock({550,450}, nullptr)
    .WithDock({550,440}, nullptr)
    .WithInventory({{1,50}})
    .Build());

  Junction* f2 = AddFactoryJunction(FactoryBuilder({y2->GetPosition().x - F_OFFSET,
    y2->GetPosition().y})
  .Capacity(3)
  .WithDock({y2->GetPosition().x - F_OFFSET, y2->GetPosition().y}, nullptr)
  .WithDock({y2->GetPosition().x - F_OFFSET,y2->GetPosition().y}, nullptr)
  .WithInventory({{2,50}})
  .Build());

  Junction* f3 = AddFactoryJunction(FactoryBuilder({y3->GetPosition().x - F_OFFSET,
    y3->GetPosition().y})
    .Capacity(3)
    .WithDock({y3->GetPosition().x - F_OFFSET, y3->GetPosition().y}, nullptr)
    .WithDock({y3->GetPosition().x - F_OFFSET, y3->GetPosition().y}, nullptr)
    .WithInventory({{1,50}})
    .Build());
  
  Junction* f4 = AddFactoryJunction(FactoryBuilder({y3->GetPosition().x + F_OFFSET,
    y3->GetPosition().y})
    .Capacity(3)
    .WithDock({y3->GetPosition().x + F_OFFSET, y3->GetPosition().y}, nullptr)
    .WithDock({y3->GetPosition().x + F_OFFSET, y3->GetPosition().y}, nullptr)
    .WithInventory({{2,50}})
    .Build());

  RoadSegment* rs1 = AddRoadSegment(j1,j2);
  RoadSegment* rs2 = AddRoadSegment(j2,j3);
  AddRoadSegment(j8,j3);


  RoadSegment* rs3 = AddRoadSegment(j1,j4);
  RoadSegment* rs4 = AddRoadSegment(j2,j5);
  RoadSegment* rs5 = AddRoadSegment(j5,j6);
  RoadSegment* rs6 = AddRoadSegment(j4,j5);

  RoadSegment* rs7 = AddRoadSegment(y1,j7);
  RoadSegment* rs8 = AddRoadSegment(y1,j6);

  RoadSegment* rs9 = AddRoadSegment(y2,j3);
  AddRoadSegment(y2,y3);
  RoadSegment* rs10 = AddRoadSegment(y3,j7);

  RoadSegment* rs11 = AddRoadSegment(j9,j10);
  RoadSegment* rs12 = AddRoadSegment(j9,j1);
  RoadSegment* rs13 = AddRoadSegment(j8,j10);

  AddRoadSegment(f1,y1);
  AddRoadSegment(f2,y2);
  AddRoadSegment(f3,y3);
  AddRoadSegment(f4,y3);

  SegmentFlush();

  Truck* tr1 = CreateTruck(rs3,0.5);
  Truck* tr2 = CreateTruck(rs5,0.5);

  tr1->AddStop({f3,f1,f2});
  
  tr2->AddStop(f2);
  return;
}

void TrafficCommand::CreateMediator() {
  mediator_ = new TrafficMediator(*this,graph_);
}

void TrafficCommand::SetUp() {
  CreateMediator();
  RoadSegmentSetUp();
  return;
  
}

TrafficCommand::TrafficCommand() {
  SetUp();
}

TrafficCommand::~TrafficCommand() {
  delete mediator_;
}

void TrafficCommand::RemoveTruck(int id) {
  trucks_.erase(id);
}

void TrafficCommand::OnTick() {
  for (auto it = junctions_.begin(); it != junctions_.end(); ++it) {
    Junction* junc = it->second.get();
    junc->OnTick();
  }
  for (auto it = trucks_.begin(); it != trucks_.end(); ++it) {
    Truck* truck = it->second.get();
    truck->OnTick();
  }
}

void TrafficCommand::Draw() {
  for (auto it = segments_.begin(); it != segments_.end(); ++it) {
    RoadSegment* rs = it->second.get();
    rs->Draw();
  }
  for (auto it = junctions_.begin(); it != junctions_.end(); ++it) {
    Junction* junc = it->second.get();
    junc->Draw();
  }
  for (auto it = trucks_.begin(); it != trucks_.end(); ++it) {
    Truck* truck = it->second.get();
    truck->Draw();
  }
}

void TrafficCommand::ConnectJunctions(Junction* j1, Junction* j2, RoadSegment* rs1) {
  if(!j1 || !j2 || !rs1) {
    TraceLog(LOG_ERROR, "Cannot connect null junction(s) or road!");
  }
  
  if(!graph_.has_edge(j1->GetGraphId(), j2->GetGraphId())) {
    graph_.add_edge(j1->GetGraphId(),j2->GetGraphId(),rs1);
  }

  if(!graph_.has_edge(j2->GetGraphId(), j1->GetGraphId())) {
    graph_.add_edge(j2->GetGraphId(),j1->GetGraphId(),rs1);
  }

  j1->AddSegmnet(rs1); 
  j2->AddSegmnet(rs1);
  rs1->SetJunctions(j1,j2); //harmless if already set
}

Truck* TrafficCommand::CreateTruck(RoadSegment* rs, Vector2 pos) {
  auto truck = std::make_unique<Truck>(pos,*mediator_);
  Truck* raw = truck.get();
  trucks_.emplace(truck->GetId(), std::move(truck));

  rs->AddTruck(raw);
  raw->SetRoadSegment(rs); 
  raw->SetJunction(rs->GetLeftSideJunction());
  return raw;
}

Truck* TrafficCommand::CreateTruck(RoadSegment* rs, float ratio) {
  if(!rs->GetLeftSideJunction() ||!rs->GetRightSideJunction()) {
      TraceLog(LOG_ERROR, "Passed incomplete road");
      return nullptr;
  }

  Vector2 p1 = rs->GetRightSideJunction()->GetPosition();
  Vector2 p2 = rs->GetLeftSideJunction()->GetPosition();

  Vector2 split_point;

  if(p1.x == p2.x) {
    split_point.y = ( (std::max(p1.y, p2.y) - std::min(p1.y,p2.y))
      * ratio) + std::min(p1.y,p2.y);
    split_point.x = p1.x;
  } else if(p1.y == p2.y) {
    split_point.x = (std::max(p1.x, p2.x) - std::min(p1.x,p2.x)) 
      * ratio;
    split_point.y = p1.y;
  } else {
    TraceLog(LOG_WARNING, "Curved or weird road!");
  }

  return CreateTruck(rs,split_point);
}

Junction* TrafficCommand::AddFourWayJunction(Vector2 position) {  
  FourWayStop* stop = new FourWayStop(position);
  auto junction = std::make_unique<Junction>(stop, JunctionType::FourWayStop);
  Junction* raw = junction.get();
  junctions_.emplace(junction->GetId(), std::move(junction));

  raw->SetGraphId(graph_.add_vertex(raw));
  return raw;
}

Junction* TrafficCommand::AddFactoryJunction(Factory* factory) {
  if (!factory) {
    TraceLog(LOG_ERROR, "Passed null factory!");
    return nullptr;
  }

  auto junction = std::make_unique<Junction>(factory, JunctionType::Factory);
  Junction* raw = junction.get();
  junctions_.emplace(junction->GetId(), std::move(junction));

  factories_.emplace(factory->GetId(), factory);
  raw->SetGraphId(graph_.add_vertex(raw));
  graaf::vertex_id_t id = graph_.add_vertex(raw);
  return raw;
}

RoadSegment* TrafficCommand::AddRoadSegment(Junction* j1, Junction* j2) {
  if(!j1 || !j2) {
    TraceLog(LOG_ERROR, "Passed null junction(s)!");
    return nullptr;
  }
  auto segment = std::make_unique<RoadSegment>(j1, j2);
  RoadSegment* raw = segment.get();
  segment_additions_.push(std::move(segment));

  ConnectJunctions(j1,j2,raw);
  return raw;
}

Junction* TrafficCommand::AddJunction(Junction* j1, Vector2 pos, 
    JunctionType type) {

  Junction* j2 = AddJunction(type,pos);
  if(j2 == nullptr) return nullptr;
  RoadSegment* rs = AddRoadSegment(j1,j2);
  return j2;
}

Junction* TrafficCommand::AddJunction(JunctionType type, Vector2 pos) {
  TrafficNode* node = nullptr;
  switch(type) {
    case JunctionType:: Factory: {
      Factory* f = FactoryBuilder(pos)
        .Capacity(3)
        .WithDock({pos.x + DOCK_WIDTH,pos.y}, nullptr)
        .WithDock({pos.x - DOCK_WIDTH,pos.y}, nullptr)
        .WithDock(pos, nullptr)
        .Build();
      return AddFactoryJunction(f);
      break;
    }
    case JunctionType::FourWayStop: {
      node = new FourWayStop(pos); 
      break;
    }
    case JunctionType::CenterYield: {
      node = new CenterYield(pos);
      break;
    }
    default:
      return nullptr;
  }

  std::unique_ptr<Junction> junction = std::make_unique<Junction>(node,type);
  Junction* raw = junction.get();
  raw->SetGraphId(graph_.add_vertex(raw));
  junctions_.emplace(raw->GetId(), std::move(junction));

  return raw;
}

Junction* TrafficCommand::AddJunction(Junction* j1, Junction* j2, 
    JunctionType type, float ratio) {

  TrafficNode* node = nullptr;
  RoadSegment* road = GetCommonRoad(j1,j2);
  Vector2 p1 = j1->GetPosition();
  Vector2 p2 = j2->GetPosition();

  Vector2 split_point;

  if(p1.x == p2.x && p1.y == p2.x) {
    return nullptr;
  } else if(p1.x == p2.x) {
    split_point = {p1.x, (p1.y + p2.y) / 2};
  } else if(p1.y == p2.y) {
    split_point = {(p1.x + p2.x) / 2, p1.y};
  } 

  Junction* junction = AddJunction(type,split_point);
  if(junction == nullptr) { return nullptr; }

  RoadSegment* road1 = AddRoadSegment(j1,junction);
  RoadSegment* road2 = AddRoadSegment(j2,junction);
  ConnectJunctions(j1,junction,road1);
  ConnectJunctions(j2,junction,road2);
  RemoveRoad(road,j1,j2);

  return junction;
}

RoadSegment* GetCommonRoad(Junction* j1, Junction* j2) {
  for (RoadSegment* rs1: j1->GetSegments()) {
    for (RoadSegment* rs2: j2->GetSegments()) {
      if (rs1 == rs2) {
        return rs1;
      }
    }
  }
  return nullptr;
}

void TrafficCommand::RemoveRoad(RoadSegment* rs1, Junction* j1, Junction* j2) {
  if(graph_.has_edge(j1->GetGraphId(), j2->GetGraphId())) {
    graph_.remove_edge(j1->GetGraphId(), j2->GetGraphId());
  }

  if(graph_.has_edge(j2->GetGraphId(), j1->GetGraphId())) {
    graph_.remove_edge(j2->GetGraphId(), j1->GetGraphId());
  }

  j1->RemoveSegment(rs1);
  j2->RemoveSegment(rs1);
  segment_deletions_.push(rs1->GetId());
}

void RemoveJunction(Junction* junction) {
  if(!junction) return;
}

void TrafficCommand::SegmentFlush() {
  while(!segment_deletions_.empty()) {
    int id = segment_deletions_.front();
    segment_deletions_.pop();
    segments_.erase(id);
  }

  while(!segment_additions_.empty()) {
    auto segment = std::move(segment_additions_.front());
    RoadSegment* raw = segment.get();
    segment_additions_.pop();
    segments_.emplace(raw->GetId(), std::move(segment));
  }
}
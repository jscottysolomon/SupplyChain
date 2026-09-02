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
#include <unordered_map>

#include <raylib.h>
#include <raymath.h>
#include <graaflib/algorithm/shortest_path/dijkstra_shortest_path.h>
#include <graaflib/algorithm/shortest_path/bfs_shortest_path.h>

#include "factory.hpp"
#include "intersection.hpp"
#include "util.hpp"
#include "traffic.hpp"
#include "traffic.hpp"
#include "truck.hpp"

#define THRESHOLD 40
#define LENGTH 150
#define Y_START 150
#define X_START 400
#define OFFSET_JUNCTION 50

void TrafficCommand::RoadSegmentSetUp() {
  int length = 150; int num = 3;


  for (float xx = X_START; xx <= X_START + (LENGTH * num); xx+=LENGTH) {
    for (float yy = Y_START; yy <= Y_START + (LENGTH * num); yy+= LENGTH) {
      Vector2 pos = {xx,yy};
      Junction* junc = AddFourWayJunction(pos);
    }
  }

  for (std::size_t xx = 0; xx < junctions_.size(); xx++) {
    for (std::size_t yy = 1; yy < junctions_.size(); yy++) {
      Junction* j1 = junctions_.at(xx);
      Junction* j2 = junctions_.at(yy);

      if(Vector2Distance(j1->GetPosition(),j2->GetPosition()) <= LENGTH 
        && j1->GetId() != j2->GetId()) {
        AddRoadSegment(j1, j2);
      }
    }
  }
  SegmentFlush();

  int index = 4;

  for (std::size_t ii = 0; ii < segments_.size(); ii++) {
    if (ii % 4 == 0) {
      Truck* tr = CreateTruck(segments_.at(ii), 0.5);
      
    }

    if (ii % index == 0) {
      index+= index;
      Junction* j1 = AddJunction(segments_.at(ii)->GetStart(), 
          segments_.at(ii)->GetEnd(),JunctionType::CenterYield, 0.5);

      if (ii % (index*index) && j1 != nullptr) {
        Vector2 p1 = j1->GetPosition();
        Junction* other = nullptr;
        for (RoadSegment* rs: j1->GetSegments()) {
          if (rs->GetStart() == j1) {
            other = rs->GetEnd();
          } else if (rs->GetEnd() == j1) {
            other = rs->GetStart();
          }

          if (other == nullptr) continue;

          Vector2 p2 = other->GetPosition();
          Vector2 p3 = p1;

          if (p1.x == p2.x) {
              p3.x += OFFSET_JUNCTION;
          } else if (p1.y == p2.y) {
              p3.y += OFFSET_JUNCTION;
          }
          AddJunction(j1, p3, JunctionType::Factory);
          break;
        }

      }
    }
  }
  SegmentFlush();



  std::unordered_map<int, int> inv = {{1,50},{2,50},{3,50},{4,50}};
  index = 0;

  for (std::size_t ii = 0; ii < trucks_.size(); ii++) {
    Truck* tr = trucks_.at(ii);

    tr->SetInventory(inv);

    if(index >= factories_.size()) {
      index = 0;
    }
    Junction* jun = graph_.get_vertex(factories_.at(index)->GetJunctionId());
    tr->AddStop(jun);
    index++;
  }

  // Junction* j1 = AddFactoryJunction(FactoryBuilder({340,420})
  //   .Capacity(3)
  //   .WithDock({340,420}, nullptr)
  //   .WithDock({340,440}, nullptr)
  //   .WithInventory(inv)
  //   .Build());

  // Junction* j2 = AddFactoryJunction(FactoryBuilder({340,420})
  //   .Capacity(3)
  //   .WithDock({340,420}, nullptr)
  //   .WithDock({340,440}, nullptr)
  //   .WithInventory(inv)
  //   .Build());

  // Junction* j3 = AddFactoryJunction(FactoryBuilder({340,420})
  //   .Capacity(3)
  //   .WithDock({340,420}, nullptr)
  //   .WithDock({340,440}, nullptr)
  //   .WithInventory(inv)
  //   .Build());

  

  // Truck* tr = 

  return;
}

void TrafficCommand::CreateMediator() {
  mediator_ = new TrafficMediator(intersections_,roads_,factories_,
    trucks_,junctions_,segments_,graph_);
}

void TrafficCommand::SetUp() {
  CreateMediator();
  RoadSegmentSetUp();
  return;
  
}

TrafficCommand::~TrafficCommand() {
  for (std::size_t ii = 0; ii < roads_.size(); ii++) {
    delete roads_.at(ii);
  }
  for (std::size_t i = 0; i < factories_.size(); i++) {
    delete factories_.at(i);
  }
  for (Truck* t:trucks_) {
    delete t;
  }
  for (Intersection* inter: intersections_) {
    delete inter;
  }

  delete mediator_;
}

void TrafficCommand::OnTick() {
  for (Factory* f: factories_) {
    f->OnTick();
  }
  for (Truck* t: trucks_) {
    t->OnTick();
  }
  for (Junction* j: junctions_) {
    j->OnTick();
  }
}

void TrafficCommand::Draw() {
  for (Road* r:roads_) {
    r->Draw();
  }
  for(RoadSegment* s: segments_) {
    s->Draw();
  }
  for (Junction* j: junctions_) {
    j->Draw();
  }
  for (Truck* t: trucks_) {
    t->Draw();
  }
}

void TrafficCommand::ConnectJunctions(Junction* j1, Junction* j2, RoadSegment* rs1) {
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
  Truck* tr = new Truck(pos,*mediator_);
  trucks_.push_back(tr);
  rs->AddTruck(tr);
  tr->SetRoadSegment(rs); 
  tr->SetJunction(rs->GetEnd());

  return tr;
}

Truck* TrafficCommand::CreateTruck(RoadSegment* rs, float ratio) {
  if(rs->GetJunctions().size() < 2)
    return nullptr;

  Vector2 p1 = rs->GetJunctions().at(0)->GetPosition();
  Vector2 p2 = rs->GetJunctions().at(1)->GetPosition();

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
    Junction* junction = new Junction(stop, JunctionType::FourWayStop);
    junctions_.push_back(junction);
    junction->SetGraphId(graph_.add_vertex(junction));
    return junction;
}

Junction* TrafficCommand::AddFactoryJunction(Factory* factory) {
  Junction* junction = new Junction(factory, JunctionType::Factory);
  junctions_.push_back(junction);
  junction->SetGraphId(graph_.add_vertex(junction));
  graaf::vertex_id_t id = graph_.add_vertex(junction);
  return junction;
}

RoadSegment* TrafficCommand::AddRoadSegment(Junction* j1, Junction* j2) {
    RoadSegment* segment = new RoadSegment({ j1->GetPosition(), j2->GetPosition() }, j1, j2);
    segment_additions_.push(segment);
    ConnectJunctions(j1,j2,segment);
    return segment;
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
      factories_.push_back(f);
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

  Junction* junction = new Junction(node,type);
  junction->SetGraphId(graph_.add_vertex(junction));
  junctions_.push_back(junction);

  return junction;
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
  segment_deletions_.push(rs1);
}

void RemoveJunction(Junction* junction) {
  if(!junction) return;
}

void TrafficCommand::SegmentFlush() {
  while(!segment_deletions_.empty()) {
    RoadSegment* rs = segment_deletions_.front();
    segment_deletions_.pop();
    segments_.erase(std::remove(segments_.begin(), segments_.end(), rs),segments_.end());
    delete rs;
    rs = nullptr;
  }

  while(!segment_additions_.empty()) {
    RoadSegment* rs = segment_additions_.front();
    segment_additions_.pop();
    segments_.push_back(rs);
  }
}
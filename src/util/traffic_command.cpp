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

void TrafficCommand::RoadSegmentSetUp() {
  int length = 150; int num = 3;


  for (int xx = X_START; xx <= X_START + (LENGTH * num); xx+=LENGTH) {
    for (int yy = Y_START; yy <= Y_START + (LENGTH * num); yy+= LENGTH) {
      Vector2 pos = {xx,yy};
      Junction* junc = AddFourWayJunction(pos);
    }
  }

  for(std::size_t xx = 0; xx < junctions_.size(); xx++) {
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

  for(std::size_t ii = 0; ii < segments_.size(); ii++) {
    if(ii % 4 == 0) {
      CreateTruck(segments_.at(ii), 0.5);
      
    }

    if(ii % index == 0) {
      index+= index;
      Junction* j1 = AddJunction(segments_.at(ii)->GetStart(), 
          segments_.at(ii)->GetEnd(),JunctionType::CenterYield, 0.5);
    }
  }
  SegmentFlush();



  std::unordered_map<int, int> inv = {{1,50},{2,50},{3,50},{4,50}};

  for (std::size_t ii = 0; ii < trucks_.size(); ii++) {
    Truck* tr = trucks_.at(ii);

    tr->SetInventory(inv);
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
  


  Road* r1 = new Road({{330,420},{330,120}});
  Road* r2 = new Road({{430,420},{430,120}});
  Road* r3 = new Road({{530,420},{530,120}});
  Road* r4 = new Road({{630,420},{630,120}});
  Road* r5 = new Road({{330,420},{630,420}});
  Road* r6 = new Road({{330,320},{630,320}});
  Road* r7 = new Road({{330,220},{630,220}});
  Road* r8 = new Road({{330,120},{630,120}});

  Truck* t1 = new Truck({330,420}, *mediator_);
  Truck* t2 = new Truck({430,320}, *mediator_);
  Truck* t3 = new Truck({530,220}, *mediator_);
  Truck* t4 = new Truck({630,120}, *mediator_);

  std::unordered_map<int, int> trcuk_inv = {{1,20},{2,20},{3,20},{4,20}};
  t1->SetInventory(trcuk_inv);
  t2->SetInventory(trcuk_inv);
  t3->SetInventory(trcuk_inv);
  t4->SetInventory(trcuk_inv);

  trucks_.push_back(t1);
  trucks_.push_back(t2);
  trucks_.push_back(t3);
  trucks_.push_back(t4);

  roads_.push_back(r1);
  roads_.push_back(r2);
  roads_.push_back(r3);
  roads_.push_back(r4);
  roads_.push_back(r5);
  roads_.push_back(r6);
  roads_.push_back(r7);
  roads_.push_back(r8);

  for (std::size_t xx = 0; xx < roads_.size(); ++xx) {
    for (std::size_t yy = xx + 1; yy < roads_.size(); ++yy) {
      Vector2 col = {};

      if (CheckCollisionLines(roads_.at(xx)->GetStart(), roads_.at(xx)->GetEnd(), roads_.at(yy)->GetStart(), roads_.at(yy)->GetEnd(), &col)) {
        roads_.at(xx)->AddRoad(roads_.at(yy));
        roads_.at(yy)->AddRoad(roads_.at(xx));
        Intersection* inter = new Intersection(col,roads_.at(xx),roads_.at(yy));
        roads_.at(xx)->AddIntersection(inter);
        roads_.at(yy)->AddIntersection(inter);
        intersections_.push_back(inter);
      }
    }

    for (Truck* t:trucks_) {
      Point p1 = {roads_.at(xx)->GetStart().x, roads_.at(xx)->GetStart().y};
      Point p2 = {roads_.at(xx)->GetEnd().x, roads_.at(xx)->GetEnd().y};
      Point p3 = {t->GetPosition().x, t->GetPosition().y};

      if (DoIntersect(p1,p2,p3,p3)) {
        t->SetCurrentRoad(roads_.at(xx));
      }
    }
  }

  for (Road* r: roads_) {
  std::vector<Intersection*> r_ints = r->GetIntersections();

  std::vector <InterComp> vec;

  for (Intersection* inter: r_ints) {
    vec.push_back(InterComp(inter, r->IsHorizontal()));
  }

  std::sort(vec.begin(), vec.end(), less_than_key());

  for (std::size_t ii = 0; ii < r_ints.size(); ii++) {
    if (ii > 0) {
      r_ints.at(ii)->AddIntersection(r_ints.at(ii-1));
      } 
      if(ii < r_ints.size() - 1) {
      r_ints.at(ii)->AddIntersection(r_ints.at(ii+1));
      }
    } 
  }

  std::unordered_map<int, int> inv = {{1,50},{2,50},{3,50},{4,50}};

  Factory* f1 = FactoryBuilder({340,420})
  .Capacity(3)
  .WithRoad(r5)
  .WithDock({340,435}, nullptr)
  .WithDock({340,440}, nullptr)
  .WithInventory(inv)
  .Build();   // bottom-left edge

  Factory* f2 = FactoryBuilder({430,330})
  .WithRoad(r6)
  .WithDock({430,345}, nullptr)
  .WithDock({430,350}, nullptr)
  .WithInventory(inv)
  .WithLines({3,4})
  .Build();   // near center

  Factory* f3 = FactoryBuilder({460,220})
  .WithRoad(r7)
  .WithDock({460,235}, nullptr)
  .WithDock({460,240}, nullptr)
  .WithInventory(inv)
  .Build();   // right edge

  Factory* f4 = FactoryBuilder({630,130})
  .Capacity(2)
  .WithRoad(r4)
  .WithDock({645,130}, nullptr)
  .WithDock({645,135}, nullptr)
  .WithInventory(inv)
  .WithLines({3,4})
  .Build();   // top-right corner

  Factory* f5 = FactoryBuilder({330,150})
  .WithRoad(r1)
  .WithDock({345,150}, nullptr)
  .WithDock({330,160}, nullptr)
  .WithInventory(inv)
  .WithLines({3,4})
  .Build();   // left edge

  Factory* f6 = FactoryBuilder({530,120})
  .WithRoad(r8)
  .WithDock({530,135}, nullptr)
  .WithDock({530,110}, nullptr)
  .WithInventory(inv)
  .WithLines({1,2})
  .Build();   // top edge

  r5->AddFactory(f1); // {340,420}
  r6->AddFactory(f2); // {430,330}
  r7->AddFactory(f3); // {460,220}
  r4->AddFactory(f4); // {630,130}
  r1->AddFactory(f5); // {330,150}
  r8->AddFactory(f6); // {530,120}

  t1->AddStop({f4,f6,f2,f5});
  t2->AddStop({f3,f1,f4,f6});
  t3->AddStop({f6,f5,f4,f3});
  t4->AddStop({f2,f4,f3,f1});

  factories_.push_back(f1);
  factories_.push_back(f2);
  factories_.push_back(f3);
  factories_.push_back(f4);
  factories_.push_back(f5);
  factories_.push_back(f6);

  Junction* j1 = AddFourWayJunction({340, 600});
  Junction* j2 = AddFourWayJunction({500, 600});
  Junction* j3 = AddFourWayJunction({340, 750});
  Junction* j4 = AddFourWayJunction({500, 750});

  Factory* f7 = FactoryBuilder({j1->GetPosition().x,j1->GetPosition().y-FACTORY_WIDTH - 100})
  // .WithDock({530,135}, nullptr)
  // .WithDock({530,110}, nullptr)
  // .WithInventory(inv)
  // .WithLines({1,2})
  .Build();
  factories_.push_back(f7);

  Junction* j5 = AddFactoryJunction(f7);

  AddRoadSegment(j1, j2);
  AddRoadSegment(j1, j3);
  AddRoadSegment(j4, j2);
  AddRoadSegment(j4, j3);
  AddRoadSegment(j1, j5);

  std::vector<Junction*> path = mediator_->RequestRoute(j1,j3);
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
  factory->SetJunctionId(junction->GetId());
  graph_.add_vertex(junction);
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
  // RoadSegment* rs3 = new RoadSegment()
//  graph_.remove_edge()
  // RoadSegment* rs3 = new RoadSegment()
  return nullptr;
}

Junction* TrafficCommand::AddJunction(JunctionType type, Vector2 pos) {
  TrafficNode* node = nullptr;
  switch(type) {
    case JunctionType:: Factory:
      node = new Factory(pos);
      break;
    case JunctionType::FourWayStop:
      node = new FourWayStop(pos);
      break;
    case JunctionType::CenterYield:
      node = new CenterYield(pos);
      break;
    default:
      return nullptr;
  }

  Junction* junction = new Junction(node,type);
  junction->SetGraphId(graph_.add_vertex(junction));
  junctions_.push_back(junction);
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
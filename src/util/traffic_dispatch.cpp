/**
 * @file traffice_manager.cpp
 * @brief This is kind of a god object that is going to be reworked to be in charge of
 * creating trucks, factories, et cetera.
 * @author J. Scotty Solomon
 * @date 12-Nov-25
 */

#include "traffic_dispatch.hpp"

#include <raylib.h>
#include <raymath.h>

#include "factory.hpp"
#include "intersection.hpp"
#include "util.hpp"
#include "traffic_control.hpp"
#include "truck.hpp"

#define THRESHOLD 40

void TrafficDispatch::SetUp() {
  controller_ = new TrafficControl(intersections_);

  Road* r1 = new Road({{330,420},{330,120}});
  Road* r2 = new Road({{430,420},{430,120}});
  Road* r3 = new Road({{530,420},{530,120}});
  Road* r4 = new Road({{630,420},{630,120}});
  Road* r5 = new Road({{330,420},{630,420}});
  Road* r6 = new Road({{330,320},{630,320}});
  Road* r7 = new Road({{330,220},{630,220}});
  Road* r8 = new Road({{330,120},{630,120}});

  Truck* t1 = new Truck({330,420}, *controller_);
  Truck* t2 = new Truck({430,320}, *controller_);
  Truck* t3 = new Truck({530,220}, *controller_);
  Truck* t4 = new Truck({630,120}, *controller_);

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

  for(std::size_t xx = 0; xx < roads_.size(); ++xx) {
    for(std::size_t yy = xx + 1; yy < roads_.size(); ++yy) {
      Vector2 col = {};

      if(CheckCollisionLines(roads_[xx]->GetStart(), roads_[xx]->GetEnd(), roads_[yy]->GetStart(), roads_[yy]->GetEnd(), &col)) {
        roads_[xx]->AddRoad(roads_[yy]);
        roads_[yy]->AddRoad(roads_[xx]);
        Intersection* inter = new Intersection(col,roads_[xx],roads_[yy]);
        roads_[xx]->AddIntersection(inter);
        roads_[yy]->AddIntersection(inter);
        intersections_.push_back(inter);
      }
    }

    for(Truck* t:trucks_) {
      Point p1 = {roads_[xx]->GetStart().x, roads_[xx]->GetStart().y};
      Point p2 = {roads_[xx]->GetEnd().x, roads_[xx]->GetEnd().y};
      Point p3 = {t->GetPosition().x, t->GetPosition().y};

      if(DoIntersect(p1,p2,p3,p3)) {
        t->SetCurrentRoad(roads_[xx]);
      }
    }
  }

  for(Road* r: roads_) {
    std::vector<Intersection*> r_ints = r->GetIntersections();

    std::vector <InterComp> vec;

    for(Intersection* inter: r_ints) {
      vec.push_back(InterComp(inter, r->IsHorizontal()));
    }

    std::sort(vec.begin(), vec.end(), less_than_key());

    for(std::size_t ii = 0; ii < r_ints.size(); ii++) {
      if(ii > 0) {
        r_ints.at(ii)->AddIntersection(r_ints.at(ii-1));
      } 
      if(ii < r_ints.size() - 1) {
        r_ints.at(ii)->AddIntersection(r_ints.at(ii+1));
      }
    }
  }

  // Factory* f1 = FactoryBuilder({340,420}).Capacity(3).WithRoad(r5).Build();   // bottom-left edge
  // Factory* f2 = FactoryBuilder({430,330}).WithRoad(r6).Build();               // near center
  // Factory* f3 = FactoryBuilder({460,220}).WithRoad(r7).Build();               // right edge
  // Factory* f4 = FactoryBuilder({630,130}).Capacity(2).WithRoad(r4).Build();   // top-right corner
  // Factory* f5 = FactoryBuilder({330,150}).WithRoad(r1).Build();               // left edge
  // Factory* f6 = FactoryBuilder({530,120}).WithRoad(r8).Build();               // top edge
  Factory* f1 = FactoryBuilder({340,420})
    .Capacity(3)
    .WithRoad(r5)
    .WithDock({340,435}, nullptr)
    .Build();   // bottom-left edge

  Factory* f2 = FactoryBuilder({430,330})
      .WithRoad(r6)
      .WithDock({430,345}, nullptr)
      .Build();   // near center

  Factory* f3 = FactoryBuilder({460,220})
      .WithRoad(r7)
      .WithDock({460,235}, nullptr)
      .Build();   // right edge

  Factory* f4 = FactoryBuilder({630,130})
      .Capacity(2)
      .WithRoad(r4)
      .WithDock({645,130}, nullptr).WithInventory({{1,90},{2,75}})
      .Build();   // top-right corner

  Factory* f5 = FactoryBuilder({330,150})
      .WithRoad(r1)
      .WithDock({345,150}, nullptr)
      .Build();   // left edge

  Factory* f6 = FactoryBuilder({530,120})
      .WithRoad(r8)
      .WithDock({530,135}, nullptr)
      .Build();   // top edge

  r5->AddFactory(f1); // {340,420}
  r6->AddFactory(f2); // {430,330}
  r7->AddFactory(f3); // {460,220}
  r4->AddFactory(f4); // {630,130}
  r1->AddFactory(f5); // {330,150}
  r8->AddFactory(f6); // {530,120}

  t1->AddStop({f4,f6,f2,f5});

  factories_.push_back(f1);
  factories_.push_back(f2);
  factories_.push_back(f3);
  factories_.push_back(f4);
  factories_.push_back(f5);
  factories_.push_back(f6);
}

TrafficDispatch::~TrafficDispatch() {
  for(std::size_t ii = 0; ii < roads_.size(); ii++) {
    delete roads_.at(ii);
  }
  for(std::size_t i = 0; i < factories_.size(); i++) {
    delete factories_.at(i);
  }
  for(Truck* t:trucks_) {
    delete t;
  }
  for(Intersection* inter: intersections_) {
    delete inter;
  }

  delete controller_;
}

void TrafficDispatch::OnTick() {
  for(Factory* f: factories_) {
    f->OnTick();
  }
  for(Truck* t: trucks_) {
    t->OnTick();
  }


}

void TrafficDispatch::Draw() {
  for(Road* r:roads_) {
    r->Draw();
  }
  for(Factory* f:factories_) {
    f->Draw();
  }
  for(Truck* t: trucks_) {
    t->Draw();
  }
}
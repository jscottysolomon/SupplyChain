/**
 * @file traffice_manager.cpp
 * @brief This is kind of a god object that is going to be reworked to assign paths to trucks
 * @author J. Scotty Solomon
 * @date 12-Nov-25
 */

#include <raylib.h>
#include <raymath.h>

#include "factory.hpp"
#include "intersection.hpp"
#include "util.hpp"
#include "traffic_dispatch.hpp"
#include "truck.hpp"

#define THRESHOLD 40

void TrafficDispatch::SetUp() {
  Road* r1 = new Road({{330,420},{330,120}});
  Road* r2 = new Road({{430,420},{430,120}});
  Road* r3 = new Road({{530,420},{530,120}});
  Road* r4 = new Road({{630,420},{630,120}});
  Road* r5 = new Road({{330,420},{630,420}});
  Road* r6 = new Road({{330,320},{630,320}});
  Road* r7 = new Road({{330,220},{630,220}});
  Road* r8 = new Road({{330,120},{630,120}});
  
  
  Factory* f1 = FactoryBuilder({340,420}).Capacity(3).Build();   // bottom-left edge
  Factory* f2 = FactoryBuilder({430,330}).Build();               // near center
  Factory* f3 = FactoryBuilder({460,220}).Build();               // right edge
  Factory* f4 = FactoryBuilder({630,130}).Capacity(2).Build();   // top-right corner
  Factory* f5 = FactoryBuilder({330,150}).Build();               // left edge
  Factory* f6 = FactoryBuilder({530,120}).Build();               // top edge

  r5->AddFactory(f1); // {340,420}
  r6->AddFactory(f2); // {430,330}
  r7->AddFactory(f3); // {460,220}
  r4->AddFactory(f4); // {630,130}
  r1->AddFactory(f5); // {330,150}
  r8->AddFactory(f6); // {530,120}

  Truck* t1 = new Truck({330,420});
  Truck* t2 = new Truck({430,320});
  Truck* t3 = new Truck({530,220});
  Truck* t4 = new Truck({630,120});

  trucks_.push_back(t1);
  trucks_.push_back(t2);
  trucks_.push_back(t3);
  trucks_.push_back(t4);

  factories_.push_back(f1);
  factories_.push_back(f2);
  factories_.push_back(f3);
  factories_.push_back(f4);
  factories_.push_back(f5);
  factories_.push_back(f6);

  roads_.push_back(r1);
  roads_.push_back(r2);
  roads_.push_back(r3);
  roads_.push_back(r4);
  roads_.push_back(r5);
  roads_.push_back(r6);
  roads_.push_back(r7);
  roads_.push_back(r8);

  for(int xx = 0; xx < roads_.size(); ++xx) {
    for(int yy = xx + 1; yy < roads_.size(); ++yy) {
      Vector2 col = {};

      if(CheckCollisionLines(roads_[xx]->GetStart(), roads_[xx]->GetEnd(), roads_[yy]->GetStart(), roads_[yy]->GetEnd(), &col)) {
        roads_[xx]->AddRoad(roads_[yy]);
        roads_[yy]->AddRoad(roads_[xx]);
        Intersection* inter = new Intersection(col,roads_[xx],roads_[yy]);
        roads_[xx]->AddIntersection(inter);
        roads_[yy]->AddIntersection(inter);
      }
    }

    for(Truck* t:trucks_) {
      Point p1 = {roads_[xx]->GetStart().x, roads_[xx]->GetStart().y};
      Point p2 = {roads_[xx]->GetEnd().x, roads_[xx]->GetEnd().y};
      Point p3 = {t->GetPosition().x, t->GetPosition().y};

      if(doIntersect(p1,p2,p3,p3)) {
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

    for(int ii = 0; ii < r_ints.size(); ii++) {
      if(ii > 0) {
        r_ints[ii]->AddIntersection(r_ints[ii-1]);
      } 
      if(ii < r_ints.size() - 1) {
        r_ints[ii]->AddIntersection(r_ints[ii+1]);
      }
    }
  }

  t1->AddStop({f4,f6,f2,f5});

}

void TrafficDispatch::OnTick() {
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

// for(Factory* f: factories_) {
    //   if(CheckCollisionPointLine(roads_[xx]->GetStart(), roads_[xx]->GetEnd(), f->GetPosition(), THRESHOLD)) {
    //     roads_[xx]->AddFactory(f);
    //   }
    // }

    //not the best implementation but it works; need to test edge cases and optimize later
    // bool intersects = false;
    // bool closest = false;
    // for(Factory* factory: factories_) {
    //   Vector2 r_start = roads_[xx]->GetStart();
    //   Vector2 r_end = roads_[xx]->GetEnd();
    //   Vector2 f_pos = factory->GetPosition();
    //   Vector2 closest;
    //   bool add = false;
      
    //   if((f_pos.x <= r_start.x && f_pos.x >= r_end.x)
    //       || (f_pos.x >= r_start.x && f_pos.x <= r_end.x)) {
    //         // add = Vector2Distance();
    //         closest.y = r_start.y;
		// 			  closest.x = f_pos.x;
    //         add = Vector2Distance(closest,f_pos) < 35;
    //   } else if((f_pos.y <= r_start.y && f_pos.y >= r_end.y)
    //       || (f_pos.y >= r_start.y && f_pos.y <= r_end.y)) {
    //         closest = r_start;
		// 			  closest.y = f_pos.y;
    //         add = Vector2Distance(closest,f_pos) < 35;
    //       }

    //   if(add) {
    //     roads_[xx]->AddFactory(factory);
    //   }
    // }
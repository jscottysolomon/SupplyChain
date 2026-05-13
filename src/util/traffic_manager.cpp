/**
 * @file traffice_manager.cpp
 * @brief 
 * @author J. Scotty Solomon
 * @date 12-Nov-25
 */

#include <raylib.h>
#include <raymath.h>

#include "factory.hpp"
#include "util.hpp"
#include "traffic_manager.hpp"
#include "truck.hpp"

void TrafficManager::SetUp() {
  Road* r1 = new Road({{50,300},{50,10}});
  Road* r2 = new Road({{300,300},{300,10}});
  Road* r3 = new Road({{50,100},{300,100}});
  Road* r4 = new Road({{50,200},{300,200}});
  // Road* r5 = new Road({{300,10},{50,300}});

  roads_.push_back(r1);
  roads_.push_back(r2);
  roads_.push_back(r3);
  roads_.push_back(r4);
  // roads_.push_back(r5);

  Factory* f = FactoryBuilder({50,20}).Capacity(3).Build();
  Factory* f2 = FactoryBuilder({150,100}).Build();
  Factory* f3 = FactoryBuilder({300,270}).Build();
  
  factories_.push_back(f);
  factories_.push_back(f2);
  factories_.push_back(f3);

  r1->AddFactory(f);
  r2->AddFactory(f2);
  r3->AddFactory(f3);  

  Truck* t1 = new Truck({50,270});
  Truck* t2 = new Truck({175,200});

  trucks_.push_back(t1);
  trucks_.push_back(t2);

  for(int xx = 0; xx < roads_.size(); ++xx) {
    for(int yy = xx + 1; yy < roads_.size(); ++yy) {
      Point p1 = {roads_[xx]->GetStart().x, roads_[xx]->GetStart().y};
      Point p2 = {roads_[xx]->GetEnd().x, roads_[xx]->GetEnd().y};
      Point p3 = {roads_[yy]->GetStart().x, roads_[yy]->GetStart().y};
      Point p4 = {roads_[yy]->GetEnd().x, roads_[yy]->GetEnd().y};
      // if(xx==yy) {continue;}

      if(doIntersect(p1,p2,p3,p4)) {
        roads_[xx]->AddRoad(roads_[yy]);
        roads_[yy]->AddRoad(roads_[xx]);
      }
    }

    for(Truck* t:trucks_) {
      Point p1 = {roads_[xx]->GetStart().x, roads_[xx]->GetStart().y};
      Point p2 = {roads_[xx]->GetEnd().x, roads_[xx]->GetEnd().y};
      Point p3 = {t->GetPosition().x, t->GetPosition().y};
      // if(xx==yy) {continue;}

      if(doIntersect(p1,p2,p3,p3)) {
        t->SetCurrentRoad(roads_[xx]);
      }
    }

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

  }

  t1->AddStop(f2);

}

void TrafficManager::OnTick() {
  for(Truck* t: trucks_) {
    t->OnTick();
  }
}

void TrafficManager::Draw() {
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
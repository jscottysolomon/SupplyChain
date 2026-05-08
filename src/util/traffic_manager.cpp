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

void TrafficManager::SetUp() {
  Road* r1 = new Road({{50,270},{50,10}});
  Road* r2 = new Road({{270,270},{270,10}});
  Road* r3 = new Road({{50,40},{270,40}});
  Road* r4 = new Road({{50,120},{270,120}});
  Road* r5 = new Road({{270,10},{50,270}});

  roads_.push_back(r1);
  roads_.push_back(r2);
  roads_.push_back(r3);
  roads_.push_back(r4);
  roads_.push_back(r5);

  Factory* f = new Factory({50,10});
  Factory* f2 = new Factory({90,40});
  Factory* f3 = new Factory({270,90});
  
  factories_.push_back(f);
  factories_.push_back(f2);
  factories_.push_back(f3);

  for(int xx = 0; xx < roads_.size(); ++xx) {
    for(int yy = xx + 1; yy < roads_.size(); ++yy) {
      Point p1 = {roads_[xx]->GetStart().x, roads_[xx]->GetStart().y};
      Point p2 = {roads_[xx]->GetEnd().x, roads_[xx]->GetEnd().y};
      Point p3 = {roads_[yy]->GetStart().x, roads_[yy]->GetStart().y};
      Point p4 = {roads_[yy]->GetEnd().x, roads_[yy]->GetEnd().y};
      // if(xx==yy) {continue;}

      if(doIntersect(p1,p2,p3,p4)) {
        roads_[xx]->AddNode(roads_[yy]);
        roads_[yy]->AddNode(roads_[xx]);
      }
    }

    //not the best implementation but it works; need to test edge cases and optimize later
    bool intersects = false;
    bool closest = false;
    for(Factory* factory: factories_) {
      Vector2 r_start = roads_[xx]->GetStart();
      Vector2 r_end = roads_[xx]->GetEnd();
      Vector2 f_pos = factory->GetPosition();
      Vector2 closest;
      bool add = false;
      
      if((f_pos.x <= r_start.x && f_pos.x >= r_end.x)
          || (f_pos.x >= r_start.x && f_pos.x <= r_end.x)) {
            // add = Vector2Distance();
            closest.y = r_start.y;
					  closest.x = f_pos.x;
            add = Vector2Distance(closest,f_pos) < 35;
      } else if((f_pos.y <= r_start.y && f_pos.y >= r_end.y)
          || (f_pos.y >= r_start.y && f_pos.y <= r_end.y)) {
            closest = r_start;
					  closest.y = f_pos.y;
            add = Vector2Distance(closest,f_pos) < 35;
          }

      if(add) {
        roads_[xx]->AddFactory(factory);
      }
    }

  }

}

void TrafficManager::Draw() {
  for(Road* r:roads_) {
    r->Draw();
  }
  for(Factory* f:factories_) {
    f->Draw();
  }
}
/**
 * @file traffice_manager.cpp
 * @brief 
 * @author J. Scotty Solomon
 * @date 12-Nov-25
 */

#include "traffic_manager.hpp"

#include "util.hpp"

void TrafficManager::SetUp() {
  Road* r1 = new Road({{50,270},{50,10}});
  Road* r2 = new Road({{270,270},{270,10}});
  Road* r3 = new Road({{50,40},{270,40}});
  Road* r4 = new Road({{50,120},{270,120}});

  roads_.push_back(r1);
  roads_.push_back(r2);
  roads_.push_back(r3);
  roads_.push_back(r4);

  for(int xx = 0; xx < roads_.size(); xx++) {
    for(int yy = 1; yy < roads_.size() - 1; yy++) {
      if(VectorsIntersect(roads_[xx]->GetStart(), roads_[xx]->GetEnd(), roads_[yy]->GetStart(), roads_[yy]->GetEnd())) {
        roads_[xx]->AddNode(roads_[yy]);
        roads_[yy]->AddNode(roads_[xx]);
      }
    }
  }
}

void TrafficManager::Draw() {
  for(Road* r:roads_) {
    r->Draw();
  }
}
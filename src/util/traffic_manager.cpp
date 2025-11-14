/**
 * @file traffice_manager.cpp
 * @brief 
 * @author J. Scotty Solomon
 * @date 12-Nov-25
 */

#include "traffic_manager.hpp"

#include "util.hpp"

void TrafficManager::SetUpIntersections() {
  Road* r1 = new Road({{10,30},{50,30}});
  Road* r2 = new Road({{30,10},{30,50}});
  Road* r3 = new Road({{10,20},{45,20}});

  roads_.push_back(r1);
  roads_.push_back(r2);
  roads_.push_back(r3);

  for(int xx = 0; xx < roads_.size(); xx++) {
    for(int yy = 1; yy < roads_.size() - 1; yy++) {
      
    }
  }
}

void TrafficManager::Draw() {
  for(Road* r:roads_) {
    r->Draw();
  }
}
#include "traffic.hpp"

#include <raylib.h>

#include "factory.hpp"

void RoadSegment::CalculateDrawingRectangle() {
  if(right_ == nullptr || left_ == nullptr) return;

  Vector2 p1 = right_->GetPosition();
  Vector2 p2 = left_->GetPosition();

  int offset = 0;

  //TODO: Factor in type of j1 & j2
  switch(left_->GetType()) {
    case JunctionType::Factory:
      offset = 0;
      break;
    case JunctionType::FourWayStop:
    case JunctionType::CenterYield:
      offset = FOUR_WAY_STOP_WIDTH * lanes_;
      break;
    default:
      offset = 0;
  }

  float road_thickness = LANE_WIDTH * lanes_;

  //Vertical
  if (p1.x == p2.x) {
    float top = std::min(p1.y, p2.y) + offset;
    float bottom = std::max(p1.y, p2.y);

    float road_length = bottom - top;
    rectangle_ = {p1.x, top, road_thickness, road_length};

    for (int ii = 1; ii < lanes_; ++ii) {
        float line_x = rectangle_.x + LANE_WIDTH * ii;

        lines_.push_back({{ line_x, rectangle_.y },
            { line_x, rectangle_.y + rectangle_.height }});
    }
  //Horizontal
  } else if (p1.y == p2.y) {
    float left = std::min(p1.x, p2.x);
    float right = std::max(p1.x, p2.x);
    left += offset;

    float road_length = right - left;
    rectangle_ = {left,p1.y,road_length,road_thickness};
    
    for (int ii = 1; ii < lanes_; ++ii) {
      float line_y = rectangle_.y + LANE_WIDTH * ii;

      lines_.push_back({{ rectangle_.x, line_y }, 
        { rectangle_.x + rectangle_.width, line_y}});
    }
  }
}

/**
 * @brief Determines which junction is on the left/right side of road
 * and adds *this to each junction.
 * 
 * @param j1 
 * @param j2 
 */
void RoadSegment::CalculateLeftRight(Junction* j1, Junction* j2) {
  if(!j1 || !j2) {
    TraceLog(LOG_ERROR, "RoadSegment pass Nullptr for j1 or j2");
    return;
  }

  if(j1->GetPosition().x == j2->GetPosition().x) {
    if(j1->GetPosition().y > j2->GetPosition(). y) {
      right_ = j1;
      left_ = j2;
    } else {
      right_ = j2;
      left_ = j1;
    }
  } else {
    if(j1->GetPosition().x > j2->GetPosition().x) {
      right_ = j1;
      left_ = j2;
    } else {
      right_ = j2;
      left_ = j1;
    }
  }

  right_->AddSegmnet(this);
  left_->AddSegmnet(this);
}


Factory* Junction::GetFactory() {
    if (auto* factory = dynamic_cast<Factory*>(obj_)) {
      return factory;
    }
    return nullptr;
}
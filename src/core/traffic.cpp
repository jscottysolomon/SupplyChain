#include "traffic.hpp"

#include "raylib.h"

void RoadSegment::CalculateDrawingRectangle() {
  if(j1_ == nullptr || j2_ == nullptr) return;

  Vector2 p1 = j1_->GetPosition();
  Vector2 p2 = j2_->GetPosition();

  int offset = 0;

  //TODO: Factor in type of j1 & j2
  switch(j2_->GetType()) {
    case JunctionType::Factory:
      offset = FACTORY_WIDTH;
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
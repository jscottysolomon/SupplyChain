/**
 * @file intersection.hpp
 * @author J. Scotty Solomon
 * @brief 
 * @version 0.1
 * @date 2026-05-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <raylib.h>
#include <vector>

#include "road.hpp"

enum class LightColor {R,Y,G}; //r,y, g already defined in raylib
enum class Direction {VERTICAL,HORIZONTAL};

class Truck;

class Intersection {
    private:
        Vector2 position_;
        Road* h_road_;
        Road* v_road_;
        LightColor h_color_;
        LightColor v_color_;
        std::vector<Intersection*> intersection_;
    public:
        Intersection(Vector2 position, Road* r1, Road*r2) {
            position_ = position;
            if(r1->GetStart().x == r1->GetEnd().x) {
                v_road_ = r1;
                h_road_ = r2;
            } else {
                v_road_ = r2;
                h_road_ = r1;
            }

            v_color_ = LightColor::G;
            h_color_ = LightColor::G;
        }
        std::vector<Road*> GetRoads() {
            return {v_road_, h_road_};
        }
        std::vector<Intersection*> GetIntersections() {return intersection_;}
        void AddIntersection(Intersection* inter) {intersection_.push_back(inter);}

        Vector2 GetPosition() {return position_;}

        void OnTick();
};

#endif
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

#include <algorithm>
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
        std::vector<Intersection*> intersections_; //adjacent intersections
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
        void AddIntersection(Intersection* intersection) {
            auto it = find(intersections_.begin(), intersections_.end(), intersection);

            // Check if the target value was found
            if (it==intersections_.end()) {
                intersections_.push_back(intersection);
            }
        }

        Vector2 GetPosition() {return position_;}
        std::vector<Intersection*> GetIntersections(){return intersections_;}
        bool HasRoad(Road* r) {return (h_road_ == r) || (v_road_ == r);}

        void OnTick();
};

struct InterComp
{
    Intersection* inter;
    bool hor;

    InterComp(Intersection* i, bool x) : inter(i), hor(x) {}

};

struct less_than_key
{
    inline bool operator() (const InterComp& struct1, const InterComp& struct2)
    {
        if(struct1.hor != struct2.hor) {
            return true;
        } else {
            if(struct1.hor) {
                return(struct1.inter->GetPosition().x < struct2.inter->GetPosition().x);
            } else {
                return(struct1.inter->GetPosition().y < struct2.inter->GetPosition().y);
            }
        }
    }
};

#endif
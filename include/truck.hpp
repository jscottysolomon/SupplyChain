/**
	truck.hpp
*/

#ifndef TRUCK_HPP
#define TRUCK_HPP

#include <raylib.h>
#include <vector>
#include <queue>

#include "entity.hpp"
#include "traffic_control.hpp"
#include "widget.hpp"

class Factory; //avoiding circular dependency
class Road;
class Intersection;

//TODO change stops to priority queue
class Truck : public Entity {
	private:
		int capacity_;		//widgets capacity
		float speed_;		//movement speed
		Road* current_road_;
		bool docked_;		//docked at factory
		Intersection* intersection_;	//current intersection
		TrafficControl& controller_;	//traffic control mediator
		Vector2 target_;				//target position
		std::vector<Widget*> widgets_;	//widgets on board
		std::vector<Factory*> stops_; 	//list of factories to go to
		std::queue<Intersection*> route_;	//route to current target factory
		void Move();
	public:
		Truck(Vector2 vec, TrafficControl& controller) : controller_(controller){
			SetPosition(vec);
			docked_ = false;
			speed_ = .05f;
			intersection_ = nullptr;
		}
		void OnTick() override;
		void AddStop(Factory* factory);
		void AddStop(std::vector<Factory*> factories);
		void ClearStops(){stops_.clear();}
		Road* GetCurrentRoad() {return current_road_;}
		void SetCurrentRoad(Road* r) {current_road_ = r;}
		void Draw() override {
			DrawRectangle(position_.x,position_.y,8,8,PINK);
		}
};

#endif
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

class Truck : public Entity {
	private:
		int capacity_;
		float speed_;
		Factory* factory_;
		Road* current_road_;
		bool docked_;
		Intersection* intersection_;
		TrafficControl& controller_;
		Vector2 target_;
		std::vector<Widget*> widgets_;
		std::vector<Factory*> stops_; 	//list of factories to go to
		std::vector<std::vector<Road*>> directions_;		//directions to current factory in list
		std::queue<Intersection*> route_;
		
		std::vector<Road*> CalculateRoute(int len, Factory* factory, std::vector<Road*> route, Road* road);
		std::vector<Road*> CalculateRoute(Factory* target, Road* start);
		void Move();
		

	public:
		Truck(Vector2 vec, TrafficControl& controller) : controller_(controller){
			SetPosition(vec);
			docked_ = false;
			factory_ = nullptr;
			speed_ = .05f;
			intersection_ = nullptr;
		}
		void OnTick() override;
		// void SetStops(std::vector<Factory*> stops) {stops_ = stops;}
		void AddStop(Factory* factory);
		void AddStop(std::vector<Factory*> factories);
		void ClearStops(){stops_.clear();}
		Road* GetCurrentRoad() {return current_road_;}
		void SetCurrentRoad(Road* r) {current_road_ = r;}
		void Draw() override {
			DrawRectangle(position_.x,position_.y,8,8,PINK);
		}
		//TODO: prioritize stop
};

class TruckMediator {

};

#endif
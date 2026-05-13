/**
	truck.hpp
*/

#ifndef TRUCK_HPP
#define TRUCK_HPP

#include <raylib.h>
#include <vector>

#include "entity.hpp"
#include "widget.hpp"

class Factory; //avoiding circular dependency
class Road;

class Truck : public Entity {
	private:
		int capacity_;
		float speed_;
		Factory* factory_;
		Road* current_road_;
		bool docked_;
		std::vector<Widget*> widgets_;
		std::vector<Factory*> stops_; 	//list of factories to go to
		std::vector<std::vector<Road*>> directions_;		//directions to current factory in list
		std::vector<Road*> CalculateRoute(int len, Factory* factory, std::vector<Road*> route, Road* road);
		void Move();

	public:
		Truck(Vector2 vec) {
			SetPosition(vec);
			docked_ = false;
			factory_ = nullptr;
			speed_ = .025f;
		}
		void OnTick() override;
		// void SetStops(std::vector<Factory*> stops) {stops_ = stops;}
		void AddStop(Factory* factory);
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
/**
	truck.hpp
*/

#ifndef TRUCK_HPP
#define TRUCK_HPP

#include <raylib.h>
#include <vector>

#include "element.hpp"
#include "widget.hpp"

class Factory; //avoiding circular dependency
class Road;

class Truck : public Element {
	private:
		int capacity_;
		double speed_;
		Factory* factory_;
		Road* current_road_;
		std::vector<Widget*> widgets_;
		std::vector<Factory*> stops_; 	//list of factories to go to
		std::vector<Road*> route_;		//directions to current factory in list
		std::vector<Road*> CalculateRoute(int len, Factory* factory, std::vector<Road*> route, Road* road);

	public:
		Truck(Vector2 vec) {
			SetPosition(vec);
		}
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
/**
	truck.hpp
*/

#ifndef TRUCK_HPP
#define TRUCK_HPP

#include <raylib.h>
#include <vector>

#include "element.hpp"
#include "road.hpp"
#include "widget.hpp"

class Factory; //avoiding circular dependency

class Truck : public Element {
	private:
		int capacity_;
		double speed_;
		Factory* factory_;
		std::vector<Widget*> widgets_;
		std::vector<Factory*> stops_; 	//list of factories to go to
		std::vector<Road*> route_;		//directions to current factory in list
	public:
		Truck(Vector2 vec) {
			SetPosition(vec);
		}
		void SetStops();
		void SetRoute();
		void Draw() override {
			DrawRectangle(position_.x,position_.y,8,8,PINK);
		}
};

class TruckMediator {

};

#endif
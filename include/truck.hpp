/**
	truck.hpp
*/

#ifndef TRUCK_HPP
#define TRUCK_HPP

#include <raylib.h>
#include <vector>
#include <queue>

#include "entity.hpp"
#include "load_planner.hpp"
#include "rules.hpp"
#include "traffic_control.hpp"
#include "widget.hpp"

class Factory; //avoiding circular dependency
class Road;
class Intersection;
struct Dock;

//TODO change stops to priority queue
class Truck : public Entity {
	private:
		//state
		int capacity_;		//widgets capacity
		float speed_;		//movement speed
		bool docked_;		//docked at factory

		//References
		Intersection* intersection_;	//current intersection
		TrafficControl& controller_;	//traffic control mediator
		Vector2 target_;				//target position
		Road* current_road_;
		Dock* dock_;

		//logic
		std::vector<Widget*> widgets_;	//widgets on board
		std::vector<Factory*> stops_; 	//list of factories to go to
		std::queue<Intersection*> route_;	//route to current target factory

		//Cargo Managmeent
		Inventory inventory_;
		LoadPlanner* unload_plan_;
		LoadPlanner* load_plan_;
		void RequestUnload();
		void Unload();
		void RequestLoad();
		void Load();
		void SetInventory(std::unordered_map<int,int> inv) {
			inventory_.SetInventory(inv);
		}
		std::unordered_map<int,int> GetInventory() {
			return inventory_.GetInventory();
		}

		//Functions
		void Move();
	public:
		Truck(Vector2 vec, TrafficControl& controller) : controller_(controller){
			SetPosition(vec);
			docked_ = false;
			speed_ = .25f;
			intersection_ = nullptr;
			dock_ = nullptr;
			load_plan_ = nullptr;
			unload_plan_ = nullptr;
		}
		//General Functions
		void OnTick() override;
		void OnDock();
		void Draw() override {
			DrawRectangle(position_.x,position_.y,8,8,PINK);
		}

		//Getters & Setters
		Road* GetCurrentRoad() {return current_road_;}
		void SetCurrentRoad(Road* r) {current_road_ = r;}
		int GetCapcity() {return inventory_.GetAvailableCapacity();}
		void AddStop(Factory* factory);
		void AddStop(std::vector<Factory*> factories);
		void ClearStops(){stops_.clear();}
		
};

#endif
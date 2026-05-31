/**
	truck.hpp
*/

#ifndef TRUCK_HPP
#define TRUCK_HPP

#include <raylib.h>
#include <vector>
#include <queue>

#include "entity.hpp"
#include "inventory.hpp"
#include "load_plan.hpp"
#include "traffic_control.hpp"
#include "widget.hpp"

class Factory; //avoiding circular dependency
class Road;
class Intersection;
struct Dock;

//TODO change stops to priority queue
class Truck : public Entity {
	public:
		Truck(Vector2 vec, TrafficControl& controller) : controller_(controller){
			SetPosition(vec);
			docked_ = false;
			speed_ = .25f;
			intersection_ = nullptr;
			dock_ = nullptr;
			dispatch_plan_ = nullptr;
			receiving_plan_ = nullptr;
			create_route = false;
		}
		//General Functions
		void OnTick() override;
		void OnDock();
		void Draw() override {
			DrawRectangle(position_.x,position_.y,8,8,PINK);
		}
		void ReceivingRequest(LoadPlan* plan, Factory* f);

		//Getters & Setters
		Road* GetCurrentRoad() {return current_road_;}
		void SetCurrentRoad(Road* r) {current_road_ = r;}
		int GetCapcity() {return inventory_.GetAvailableCapacity();}
		void AddStop(Factory* factory);
		void AddStop(std::vector<Factory*> factories);
		void ClearStops(){
			while(!stops_.empty()) {
				stops_.pop();
			}
		}
		void SetInventory(std::unordered_map<int,int> inv) {
			inventory_.SetInventory(inv);
		}
		std::unordered_map<int,int> GetInventory() {
			return inventory_.GetInventory();
		}

		void SetUnloaderInventory(LoadPlan* plan) {
			plan->SetUnloaderInventory(&inventory_);
		}
	private:
		//state
		int capacity_;		//widgets capacity
		float speed_;		//movement speed
		bool docked_;		//docked at factory
		bool create_route;

		//References
		Intersection* intersection_;	//current intersection
		TrafficControl& controller_;	//traffic control mediator
		Vector2 target_;				//target position
		Road* current_road_;
		Dock* dock_;

		//logic
		std::vector<Widget*> widgets_;	//widgets on board
		std::queue<Factory*> stops_; 	//list of factories to go to
		std::queue<Intersection*> route_;	//route to current target factory

		//Cargo Managmeent
		Inventory inventory_;
		LoadPlan* receiving_plan_;	//Received from truck to factory
		LoadPlan* dispatch_plan_;	//Dispatched from factory to truck
		void Receive();
		void RequestDispatch();
		void Dispatch();
		

		//Functions
		void Move();
};

#endif
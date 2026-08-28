/**
	truck.hpp
*/

#ifndef TRUCK_HPP
#define TRUCK_HPP

#include <raylib.h>
#include <vector>
#include <queue>

#include "common.hpp"
#include "entity.hpp"
#include "inventory.hpp"
#include "rules.hpp"
#include "traffic.hpp"
#include "traffic.hpp"
#include "widget.hpp"

class Factory; //avoiding circular dependency
class Road;
class Intersection;
struct Dock;

enum TruckState {
	kDriving,
	kReceiving,
	kDispatching,
	kStalling
};

//TODO change stops to priority queue
class Truck : public MapObject {
	public:
		Truck(Vector2 vec, TrafficMediator& controller) : MapObject(vec), mediator_(controller) {
			SetPosition(vec);
			docked_ = false;
			speed_ = .025f;
			intersection_ = nullptr;
			dock_ = nullptr;
			create_route = false;
			state_ = kDriving; //TODO: update
			capacity_ = 500;
		}
		//General Functions
		void OnTick() override;
		void Draw() override {
			DrawRectangle(position_.x,position_.y,TRUCK_WIDTH,TRUCK_WIDTH,PINK);
		}
		//Getters & Setters
		Road* GetCurrentRoad() { return current_road_; }
		void SetCurrentRoad(Road* r) { current_road_ = r; }

		std::vector<Factory*> GetStops() { return stops_; }
		std::vector<Factory*> GetSchedule() { return schedule_;}
		
		void AddStop(Factory* factory);
		void AddStop(std::vector<Factory*> factories);

		void ClearStops() {
			while (!stops_.empty()) {
				stops_.erase(stops_.begin());
			}
		}

		/*Inventory Wrapper*/
		void SetInventory(std::unordered_map<int,int> inv) { inventory_.SetInventory(inv); }
		std::unordered_map<int,int> GetInventoryMap() { return inventory_.GetInventoryMap(); }
		Inventory* GetInventory() { return &inventory_; }
		int GetWidgetQuantity(int id) { return inventory_.GetWidgetQuantity(id); }
		int GetMaxCapacity() {return inventory_.GetMaxCapacity(); }
		int GetAvailableCapacity() { return inventory_.GetAvailableCapacity(); }

		bool IsState(TruckState state) { return state_ == state; }

		Plan* GetPlan(int id) {
			if (plans_.find(id) != plans_.end()) {
				return plans_.at(id);
			}

			return nullptr;
		}

		RuleContext& GetContext(int id) { return contexts_.at(id); }
		void RemoveTarget(Target* t, int factory_id) {
			Plan* p = plans_.at(factory_id);
			
			p->RemoveTarget(t);
		}
	private:
		/*Internal State*/
		int capacity_;		//widgets capacity
		float speed_;		//movement speed
		bool docked_;		//docked at factory
		bool create_route;
		TruckState state_;
		
		void SetState(TruckState state) {
			state_ = state;
			switch (state) {
				case kDriving:
					docked_ = false;
					break;
				case kDispatching:
				case kReceiving:
					docked_ = true;
					break;
				default:
					break;
			}
		}

		/*Road Relations*/
		Intersection* intersection_;	//current intersection
		TrafficMediator& mediator_;	//traffic control mediator
		RoadSegment* segment_;
		Vector2 target_;				//target position
		Road* current_road_;
		Dock* dock_;

		/*Schedules and Routes*/
		std::vector<Factory*> stops_; 		//dynamic list of stops
		std::vector<Factory*> schedule_; 	//fixed schedule
		std::queue<Intersection*> route_;	//route to current target factory

		/*Cargo Management*/
		Inventory inventory_;
		std::unordered_map<int,RuleContext> contexts_; //id, context
		std::unordered_map<int, Plan*> plans_;
		void Receive();
		void Dispatch();
		void Stall();
		void Drive();		

		//Functions
};

class TruckBuilder {
public:
	TruckBuilder(Vector2 vec, TrafficMediator& controller) {
		truck_ = new Truck(vec, controller);
	}

	Truck* Build() {return truck_;}

	TruckBuilder& Capacity(int capacity) {
		return *this;
	}

	TruckBuilder& WithInventory (std::unordered_map<int,int> inv) {
		truck_->SetInventory(inv);
		return *this;
	}

	TruckBuilder& WithStop(Factory* factory);


private:
	Truck* truck_;
};

#endif
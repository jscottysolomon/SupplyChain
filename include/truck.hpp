/**
	truck.hpp
*/

#ifndef TRUCK_HPP
#define TRUCK_HPP

#include <list>
#include <vector>
#include <queue>

#include <raylib.h>
#include <raymath.h>

#include "common.hpp"
#include "entity.hpp"
#include "inventory.hpp"
#include "rules.hpp"
#include "traffic.hpp"
#include "widget.hpp"

class Factory; //avoiding circular dependency
class Road;
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
			junction_ = nullptr;
			dock_ = nullptr;
			create_route = false;
			state_ = kDriving; //TODO: update
			capacity_ = 500;
			target_ = {-1,-1};
			right_side_ = true;
		}
		~Truck() override = default;
		//General Functions
		void OnTick() override;
		void Draw() override {
			DrawRectangle(position_.x,position_.y,TRUCK_WIDTH,TRUCK_WIDTH,PINK);
		}
		//Getters & Setters
		void SetRoadSegment(RoadSegment* rs) 
			{ segment_ = rs; }
		RoadSegment* GetRoadSegment() 
			{ return segment_; }
		std::vector<Junction*> GetStops()
			{ return stops_; }
		std::vector<Junction*> GetSchedule()
			{ return schedule_;}
		void SetJunction(Junction* junc)
			{ junction_ = junc; }
		Junction* GetJunction() 
			{ return junction_; }
		
		void AddStop(Junction* junction);
		void AddStop(std::vector<Junction*> junctions);

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

		/**
		 * @brief If truck is travelling on the right or left of the road. 
		 * 
		 * @return true 
		 * @return false 
		 */
		bool IsOnRightLane()
			{return right_side_;}

		Plan* GetPlan(int id) {
			if (plans_.find(id) != plans_.end()) {
				return plans_.at(id);
			}

			return nullptr;
		}

		RuleContext& GetContext(int id) { 
			return contexts_[id];
		}
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
		Junction* junction_;
		TrafficMediator& mediator_;	//traffic control mediator
		RoadSegment* segment_;
		Vector2 target_;				//target position
		Dock* dock_;
		bool right_side_;

		/*Schedules and Routes*/
		std::vector<Junction*> stops_; 		//dynamic list of stops
		std::vector<Junction*> schedule_; 	//fixed schedule
		std::list<Junction*> route_;	//route to current target factory

		/*Cargo Management*/
		Inventory inventory_;
		std::unordered_map<int,RuleContext> contexts_; //id, context
		std::unordered_map<int, Plan*> plans_;
		void Receive();
		void Dispatch();
		void Stall();
		void Drive();		

		bool HasValidTarget() {
			return target_.x > 0 && target_.y > 0;
		}

		void SetTarget(Vector2 pos) {
			target_ = pos;
		}

		bool DeriveNextTarget() {
			if(!stops_.empty()) {
				// junction_ = stops_.front();
				target_ = junction_->GetPosition();
				return true;
			} else {
				SetTarget({-1,-1});
				// junction_ = nullptr;
				return false;
			}
		}

		void MoveToTarget() {
			Vector2 movement_vector = Vector2Subtract(target_, position_);
			Vector2 movement = Vector2Scale(movement_vector, speed_);
			SetPosition(Vector2Add(position_, movement));
		}

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
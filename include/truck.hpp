/**
	truck.hpp
*/

#ifndef TRUCK_HPP
#define TRUCK_HPP

#include <list>
#include <vector>
#include <queue>

#include <nlohmann/json.hpp>
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
	kDriving = 0,
	kReceiving,
	kDispatching,
	kStalling
};

//TODO change stops to priority queue
class Truck : public MapObject {
	public:
		Truck() = default;
		Truck(Vector2 vec, TrafficMediator* controller) : MapObject(vec) {
			SetPosition(vec);
			mediator_ = controller;
			docked_ = false;
			speed_ = .025f;
			create_route = false;
			state_ = kDriving; //TODO: update
			pallete_capacity = 500;
			target_ = {-1,-1};
			right_side_ = true;
		}
		~Truck() override = default;
		//General Functions
		void OnTick() override;
		void Draw() override {
			DrawRectangle(position_.x,position_.y,TRUCK_WIDTH,TRUCK_WIDTH,PINK);
		}
		
		int GetDockId() const 
			{ return dock_id_;}
		void SetDockId(int id) 
			{dock_id_ = id;}
		int GetJunctionId() const
			{ return junction_id_;}
		void SetJunctionId(int id) 
			{ junction_id_ = id;}
		int GetSegmentId() const
			{ return segment_id_;}
		void SetRoadSegmentId(int id)
			{segment_id_ = id;}
		std::vector<int> GetDynamicSchedule() 
			{ return dynamic_schedule_ids_; }
		std::vector<int> GetFixedSchedule()
			{ return fixed_schedule_ids_;}
		std::list<int> GetPathway() 
			{ return pathway_ids_; }
		
		void AddStop(Junction* junction);
		void AddStop(std::vector<Junction*> junctions);

		void ClearStops() {
			while (!dynamic_schedule_ids_.empty()) {
				dynamic_schedule_ids_.erase(dynamic_schedule_ids_.begin());
			}
		}

		/*Inventory Wrapper*/
		void SetInventory(std::unordered_map<int,int> inv) { inventory_.SetInventory(inv); }
		std::unordered_map<int,int> GetInventoryMap() const
			{ return inventory_.GetInventoryMap(); }
		const Inventory* GetInventory() const
			{ return &inventory_; }
		Inventory* GetInventory()
			{ return &inventory_; }
		int GetWidgetQuantity(int id) const
			{ return inventory_.GetWidgetQuantity(id); }
		int GetMaxCapacity() const
			{return inventory_.GetMaxCapacity(); }
		int GetAvailableCapacity() const 
			{ return inventory_.GetAvailableCapacity(); }

		bool IsState(TruckState state) { return state_ == state; }

		/**
		 * @brief If truck is travelling on the right or left of the road. 
		 * 
		 * @return true 
		 * @return false 
		 */
		bool IsOnRightLane() const
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

		NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Truck,pallete_capacity,speed_,docked_,create_route,state_)
	private:
		/*Internal State*/
		int pallete_capacity = 10;		//widgets capacity
		float speed_ = 0.25f;		//movement speed
		TruckState state_ = TruckState::kDriving;
		bool right_side_ = false;
		bool docked_ = false;		//docked at factory
		bool create_route = false;
		Vector2 target_ = {-1,-1}; //target position

		/*Relative Ids*/
		int junction_id_ = -1;
		int segment_id_ = -1;
		int dock_id_ = -1;
		std::vector<int> dynamic_schedule_ids_ = {};
		std::vector<int> fixed_schedule_ids_ = {};
		std::list<int> pathway_ids_ = {};

		TrafficMediator* mediator_;	//traffic control mediator

		/*Cargo Management*/
		Inventory inventory_;
		std::unordered_map<int,RuleContext> contexts_; //id, context
		std::unordered_map<int, Plan*> plans_;

		/*Road Relations*/
		// Junction* junction_;
		// RoadSegment* segment_;
		// Dock* dock_;
		// std::vector<Junction*> dynamic_schedule_; 		//dynamic list of stops
		// std::vector<Junction*> fixed_schedule_; 	//fixed schedule
		// std::list<Junction*> pathway;	//route to current target factory

		/*Schedules and Routes*/
		

		
		void Receive();
		void Dispatch();
		void Stall();
		void Drive();		
		bool DeriveNextTarget();

		bool HasValidTarget() 
			{ return target_.x > 0 && target_.y > 0; }

		void SetTarget(Vector2 pos) 
			{ target_ = pos; }


		void MoveToTarget() {
			Vector2 movement_vector = Vector2Subtract(target_, position_);
			Vector2 movement = Vector2Scale(movement_vector, speed_);
			SetPosition(Vector2Add(position_, movement));
		}

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

		//Functions
};

#endif
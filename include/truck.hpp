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
#include "util.hpp"
#include "widget.hpp"

class Factory; //avoiding circular dependency
class Inventory;
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
		
		void AddToSchedule(Junction* junction);
		void AddToSchedule(std::vector<Junction*> junctions);

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
		int GetWidgetPalletQuantity(int id) const
			{ return inventory_.GetWidgetPalletQuantity(id); }
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

		int GetPlan(int factory_id) const
			{ return plans_.at(factory_id); }
		void AddPlan(int factory_id, int plan_id)
			{ plans_.emplace(factory_id,plan_id);}
		
		NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Truck,id_,position_,speed_,state_,
			right_side_,docked_,create_route,target_,junction_id_,segment_id_,dock_id_,dynamic_schedule_ids_,
			fixed_schedule_ids_, pathway_ids_,inventory_)
	private:
		/*Internal State*/
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
		std::unordered_map<int,int> plans_; //{FactoryID, PlanID}
		std::vector<int> dynamic_schedule_ids_ = {};
		std::vector<int> fixed_schedule_ids_ = {};
		std::list<int> pathway_ids_ = {};

		TrafficMediator* mediator_;	//traffic control mediator

		/*Cargo Management*/
		Inventory inventory_;
		std::unordered_map<int, RuleContext> contexts_; //id, context
		
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
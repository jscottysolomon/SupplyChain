/**
Factory.hpp
*/

#include <vector>
#include <raylib.h>

#include "entity.hpp"
#include "load_planner.hpp"
#include "rules.hpp"
#include "widget.hpp"

#ifndef FACTORY_HPP
#define FACTORY_HPP

class Truck; //avoiding circular dependency
class Road;
class Intersection;

struct Dock {
	Vector2 position;
	Truck* truck;
	bool assigned;
	bool cargo_ready;
	// LoadPlan load_plan;
	LoadPlan unload_plan;
};

class Factory : public Entity {
	public:
		Factory(Vector2 points) {
			SetPosition(points);
			intersection_ = nullptr;
			load_speed_ = 2;
		}
		~Factory() {
			for(Dock* dock: docks_) {
				delete dock;
			}
		}

		//Mediator functions

		/* Mediated truck requested dock station */
		Dock* DockRequest(Truck* truck);

		//Inventory & Docking management
		

		/*Truck requesting to undock from factory*/
		Inventory inventory_;
		float load_speed_;
		void UnloadRequest(Truck* truck, LoadPlanner* plan);
		// LoadPlan LoadRequest(Truck* truck, LoadPlan plan);

		/*Telling Truck to undock*/
		void Undock(Truck* t);


		std::unordered_map<int,int> Unload(Truck* t);


		void OnTick() override;

		void AddDock(Vector2 pos, Truck* t){
			struct Dock* dock = new Dock;

			dock->position = pos;
			dock->truck = t;
			dock->assigned = (t != nullptr);

			docks_.push_back(dock);
		}

		//Will return nullptr if not found
		Dock* GetDock(Truck* t) {
			for(Dock* dock: docks_) {
				if(dock->truck == t) {
					return dock;
				}
			}
			return nullptr;
		}
		std::vector<Dock*> GetDocks() 
			{return docks_;}
		void IncreaseDockCapacity()
			{dock_capcity_++;}
		void SetCapacity(int capacity) 
			{dock_capcity_ = capacity;}
		void SetRoad(Road* r) 
			{road_ = r;}
		Road* GetRoad() 
			{return road_;}
		void SetIntersection(Intersection* i) 
			{intersection_ = i;}
		Intersection* GetIntersection() 
			{return intersection_;}
		bool HasEmptyDock();
		std::unordered_map<int,int> GetInventory() {
			return inventory_.GetInventory();
		}
		void SetInventory(std::unordered_map<int,int> inv) {
			inventory_.SetInventory(inv);
		}
		void SetPlanInventory(LoadPlanner* plan, bool receiver) {
			if(receiver) {
				plan->SetReceiverInventory(&inventory_);
			} else {
				plan->SetUnloaderInventory(&inventory_);
			}
		}

		void Draw() override {
			DrawRectangle(position_.x,position_.y,15,15,BLUE);

			for(Dock* d: docks_) {
				DrawRectangle(position_.x,position_.y,10,10,RED);
			}
		}

	private:
		int dock_capcity_;
		int widget_capacity_;
		std::vector<Dock*> docks_;
		Intersection* intersection_;
		Road* road_;
		std::vector<int> craftables_;
		std::vector<int> loadables_;
};

class FactoryBuilder {
	private:
		Factory* factory;

	public:
		FactoryBuilder(Vector2 vec) {
			factory = new Factory(vec);
			factory->SetCapacity(1);
		}

		Factory* Build() {return factory;}

		FactoryBuilder& Capacity(int capcity) {
			factory->SetCapacity(capcity);
			return *this;
		}

		FactoryBuilder& WithRoad(Road* road);

		FactoryBuilder& WithDock(Vector2 pos, Truck* truck) {
			factory->AddDock(pos,truck);
			return *this;
		}

		FactoryBuilder& WithInventory (std::unordered_map<int,int> inv) {
			factory->SetInventory(inv);
			return *this;
		}
};

#endif
/**
Factory.hpp
*/

#include <vector>
#include <raylib.h>
#include <time.h>

#include "entity.hpp"
#include "inventory.hpp"
#include "load_plan.hpp"
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
	LoadPlan* dispatch_plan;
	LoadPlan* receiving_plan;
};

struct ProductionLine {
	clock_t last_production;
	int id;
};

class Factory : public Entity {
	public:
		Factory(Vector2 points) {
			SetPosition(points);
			intersection_ = nullptr;
			load_speed_ = 2;
			organizer = ReceipeOrganizer::GetInstance();
		}
		~Factory() {
			for(Dock* dock: docks_) {
				delete dock;
			}
		}

		//Mediator functions
		Dock* DockRequest(Truck* truck);
		void Undock(Truck* t);
		void DispatchRequest(Truck* truck, LoadPlan* plan);
		void RequestReceiving(Dock* d);
		std::unordered_map<int,int> Unload(Truck* t);		

		//Getter, Setters, Check State, Et cetera
		std::vector<Dock*> GetDocks() {return docks_;}
		void IncreaseDockCapacity() {dock_capcity_++;}
		void SetCapacity(int capacity) {dock_capcity_ = capacity;}
		void SetRoad(Road* r) {road_ = r;}
		Road* GetRoad() {return road_;}
		void SetIntersection(Intersection* i) {intersection_ = i;}
		Intersection* GetIntersection() {return intersection_;}
		std::unordered_map<int,int> GetInventoryMap()
			{return inventory_.GetInventoryMap();}
		Inventory* GetInventory() {
			return &inventory_;
		}
		void SetInventory(std::unordered_map<int,int> inv) 
			{
				inventory_.SetInventory(inv);
			}
		bool HasEmptyDock();

		Dock* GetDock(Truck* t) {
			for(Dock* dock: docks_) {
				if(dock->truck == t) {
					return dock;
				}
			}
			return nullptr;
		}

		void AddDock(Vector2 pos, Truck* t){
			struct Dock* dock = new Dock;

			dock->position = pos;
			dock->truck = t;
			dock->assigned = t; //nullptr if t=nullptr
			dock->receiving_plan = nullptr;
			dock->dispatch_plan = nullptr;
			dock->cargo_ready = false;

			docks_.push_back(dock);
		}

		void AddProductionLine(int id) {
			production_lines_.push_back({-1,id});
		}
		
		void SetPlanInventory(LoadPlan* plan, bool receiver) {
			if(receiver) {
				plan->SetReceiverInventory(&inventory_);
			} else {
				plan->SetUnloaderInventory(&inventory_);
			}
		}

		//Entity Functions
		void Draw() override {
			DrawRectangle(position_.x,position_.y,15,15,BLUE);

			for(Dock* d: docks_) {
				DrawRectangle(position_.x,position_.y,10,10,RED);
			}
		}
		void OnTick() override;
		
		std::vector<ProductionLine> GetProductionLines() {
			return production_lines_;
		}

		std::set<int> GetRecipeIds() {
			std::set<int> ret;
			for(ProductionLine line: production_lines_) {
				for(std::pair<int,int> p: organizer->GetRecipe(line.id)) {
					ret.insert(p.first);
				}
			}

			return ret;
		}

	private:
		int dock_capcity_;
		int widget_capacity_;
		float load_speed_;
		std::vector<int> craftables_;
		std::vector<int> loadables_;
		std::vector<Dock*> docks_;
		Intersection* intersection_;
		Road* road_;
		Inventory inventory_;
		std::vector<ProductionLine> production_lines_;
		ReceipeOrganizer* organizer;

		void Produce();
		void DockOnTick();
};

class FactoryBuilder {
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

	FactoryBuilder& WithLine (int id) {
		factory->AddProductionLine(id);
		return *this;
	}

	FactoryBuilder& WithLines (std::vector<int> ids) {
		for(int id: ids) {
			factory->AddProductionLine(id);
		}
		return *this;
	}
private:
	Factory* factory;
};

#endif
/**
Factory.hpp
*/

#include <vector>
#include <raylib.h>
#include <time.h>

#include "entity.hpp"
#include "inventory.hpp"
#include "traffic.hpp"
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
};

struct ProductionLine {
	double last_production;		//last time something was procduced
	double production_start; 	//when productino of item started
	float efficienty = 1.0;
	int id;
};

class Factory : public TrafficNode {
	public:
		Factory(Vector2 points): TrafficNode(points) {
			// SetPosition(points);
			intersection_ = nullptr;
			load_speed_ = 2;
			organizer = ReceipeOrganizer::GetInstance();
		}
		~Factory() {
			for (Dock* dock: docks_) {
				delete dock;
			}
		}

		//Mediator functions
		Dock* DockRequest(Truck* truck);
		void Undock(Truck* t);

		//Getter, Setters, Check State, Et cetera
		std::vector<Dock*> GetDocks() 
			{return docks_;}
		void IncreaseDockCapacity() 
			{dock_capcity_++;}
		void SetDockQuantity(int capacity) 
			{dock_capcity_ = capacity;}
		void SetRoad(Road* r) 
			{road_ = r;}
		Road* GetRoad() 
			{return road_;}
		void SetRoadSegment(RoadSegment* segment)
			{segment_ = segment;}
		RoadSegment* GetRoadSegment() 
			{return segment_;}
		void SetIntersection(Intersection* i) 
			{intersection_ = i;}
		Intersection* GetIntersection() 
			{return intersection_;}
		std::unordered_map<int,int> GetInventoryMap()
			{return inventory_.GetInventoryMap();}

		/*Inventory Wrapper*/
		Inventory* GetInventory() { return &inventory_; }
		void SetInventory(std::unordered_map<int,int> inv) { inventory_.SetInventory(inv); }
		int GetAvailableCapacity() { return inventory_.GetAvailableCapacity(); }
		int GetWidgetQuantity(int id) { return inventory_.GetWidgetQuantity(id); }
		int GetMaxCapacity() { return inventory_.GetMaxCapacity(); }

		Dock* GetDock(Truck* t) {
			for (Dock* dock: docks_) {
				if (dock->truck == t) {
					return dock;
				}
			}
			return nullptr;
		}

		void AddDock(Vector2 pos, Truck* t) {
			struct Dock* dock = new Dock;

			dock->position = pos;
			dock->truck = t;
			dock->assigned = t; //nullptr if t=nullptr
			dock->cargo_ready = false;

			docks_.push_back(dock);
		}

		void AddProductionLine(int id) {
			production_lines_.push_back({-1,-1,1.0,id});
		}

		//MapObject Functions
		void Draw() override {
			DrawRectangle(position_.x,position_.y,FACTORY_WIDTH,FACTORY_WIDTH,BLUE);

			for (Dock* d: docks_) {
				DrawRectangle(position_.x,position_.y,DOCK_WIDTH,DOCK_WIDTH,RED);
			}
		}
		void OnTick() override;
		
		std::vector<ProductionLine> GetProductionLines() {
			return production_lines_;
		}

		std::set<int> GetRecipeIds() {
			std::set<int> ret;
			for (ProductionLine line: production_lines_) {
				for (std::pair<int,int> p: organizer->GetRecipe(line.id)) {
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
		RoadSegment* segment_;

		void Produce();
		void DockOnTick();
};

class FactoryBuilder {
public:
	FactoryBuilder(Vector2 vec) {
		factory = new Factory(vec);
		factory->SetDockQuantity(1);
	}

	Factory* Build() {return factory;}

	FactoryBuilder& Capacity(int capcity) {
		factory->SetDockQuantity(capcity);
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
		for (int id: ids) {
			factory->AddProductionLine(id);
		}
		return *this;
	}
private:
	Factory* factory;
};

#endif
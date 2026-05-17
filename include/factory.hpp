/**
Factory.hpp
*/

#include <vector>
#include <raylib.h>

#include "entity.hpp"

#ifndef FACTORY_HPP
#define FACTORY_HPP

class Truck; //avoiding circular dependency
class Road;
class Intersection;

struct Dock {
	Vector2 position;
	bool occupied;
	Truck* truck;
};

class Factory : public Entity {
	private:
		int capacity_;
		std::vector<Dock*> docks_;
		Intersection* intersection_;
		Road* road_;
	public:
		Factory(Vector2 points) {
			SetPosition(points);
			intersection_ = nullptr;
		}
		~Factory() {
			for(Dock* dock: docks_) {
				delete dock;
			}
		}
		bool HasEmptyDock();
		Dock* DockRequest(Truck* truck);
		void OnTick() override;

		std::vector<Dock*> GetDocks() {return docks_;}
		void AddDock(Vector2 pos, Truck* t){
			struct Dock* dock = new Dock;

			dock->position = pos;
			dock->truck = t;
			dock->occupied = (t != nullptr);

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


		void IncreaseDockCapacity(){capacity_++;}
		void SetCapacity(int capacity) {capacity_ = capacity;}
		void SetRoad(Road* r) {road_ = r;}
		Road* GetRoad() {return road_;}
		void SetIntersection(Intersection* i) {intersection_ = i;}
		Intersection* GetIntersection() {return intersection_;}

		void Draw() override {
			DrawRectangle(position_.x,position_.y,15,15,BLUE);

			for(Dock* d: docks_) {
				DrawRectangle(position_.x,position_.y,10,10,RED);
			}
		}
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


};

#endif
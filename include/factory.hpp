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
		bool HasEmptyDock();
		std::vector<Dock*> GetDocks() {return docks_;}
		void IncreaseDockCapacity(){capacity_++;}
		void SetCapacity(int capacity) {capacity_ = capacity;}
		void SetRoad(Road* r) {road_ = r;}
		Road* GetRoad() {return road_;}
		void SetIntersection(Intersection* i) {intersection_ = i;}
		Intersection* GetIntersection() {return intersection_;}
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

};

#endif
/**
Factory.hpp
*/

#include <vector>
#include <raylib.h>

#include "element.hpp"

#ifndef FACTORY_HPP
#define FACTORY_HPP

class Truck; //avoiding circular dependency

struct Dock {
	Vector2 position;
	bool occupied;
	Truck* truck;
};

class Factory : public Element {
	private:
		int capacity_;
		std::vector<Dock> docks_;
	public:
		Factory(Vector2 points) {
			SetPosition(points);
		}
		bool HasEmptyDock();

};

#endif
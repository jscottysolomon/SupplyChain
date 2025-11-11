/**
Factory.hpp
*/

#include <vector>

#include "element.hpp"

#ifndef FACTORY_HPP
#define FACTORY_HPP

class Truck;

struct Dock {
	Vector2 position;
	bool occupied;
	Truck* truck;
};

class Factory : public Element {
	private:
		int capacity;
		std::vector<Dock> docks;
	public:
		bool HasEmptyDock();

};

#endif
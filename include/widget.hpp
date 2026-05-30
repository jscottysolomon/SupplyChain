/*
widget.hpp
*/

#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <set>

struct Recipe {
    int widget_id;
    int output_amount;
    std::unordered_map<int, int> inputs; //id,quantity
	int production_time;
};

struct Widget {
	int id;
	int capacity_unit = 1;
	std::string name;
	Recipe recipe;
};



#endif
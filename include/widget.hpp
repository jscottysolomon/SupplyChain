/*
widget.hpp
*/

#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <set>

#include "inventory.hpp"

#define BASE_PRODUCTION_TIME 5

struct Widget {
	int id;
	int capacity_unit = 1;
	std::string name;
	std::unordered_map<int, int> inputs; //id,quantity
	int output_amount;
	float production_time;
};

class ReceipeOrganizer {
public:
	ReceipeOrganizer() {
		next_id_ = 0;
		SetUp();
	}

	int CreateWidget(std::string name) {
		Widget w = {next_id_++,1,name,{},1,BASE_PRODUCTION_TIME};
		widgets_[w.id] = w;
		return w.id;
	}

	int CreateWidget(std::string name, std::unordered_map<int,int> inputs, int output, float time) {
		Widget w = {next_id_++,1,name,inputs,output,time};
		widgets_[w.id] = w;
		return w.id;
	}

	int CreateWidget(std::string name, std::unordered_map<int,int> inputs, float time) {
		Widget w = {next_id_++,1,name,inputs,1,time};
		widgets_[w.id] = w;
		return w.id;
	}

	Widget GetWidget(int id) {
		//TODO: Check if widget exists
		return widgets_[id];
	}

	void SetReceipe(int id, std::unordered_map<int,int> recipe) {
		widgets_[id].inputs = recipe;
	}

	float GetTotalProductionTime(int id, float total_time) {
		int time = widgets_[id].production_time;
		// for(std::pair<int, int> p: widgets_[id].inputs) {
		// 	// time += 
		// }

		return 0;
	}

	float GetProductionTime(int id) {
		return widgets_[id].production_time;
	}

	static ReceipeOrganizer* GetInstance() {
        if (obj == nullptr)
            obj = new ReceipeOrganizer();
        return obj;
    }

	bool ProduceWidget(Inventory* inv, int id) {
		bool satisfied = true;
		
		//Checking inventory has required amount for widgets
		for(std::pair<int, int> p: widgets_[id].inputs) {
			if(inv->GetWidgetQuantity(p.first) < p.second) {
				satisfied = false;
			}
		}

		if(!satisfied) return satisfied;

		for(std::pair<int, int> p: widgets_[id].inputs) {
			inv->RemoveWidget(p.first,p.second);
		}

		inv->AddWidget(id,widgets_[id].output_amount);

		return satisfied;
	}

private:
    static ReceipeOrganizer* obj;
	void SetUp() {
		int id_1 = CreateWidget("Widget A");
		int id_2 = CreateWidget("Widget B");
		CreateWidget("Widget C",{{id_1,2}},BASE_PRODUCTION_TIME*1.1);
		CreateWidget("Widget D",{{id_2,2}},BASE_PRODUCTION_TIME*1.2);
		CreateWidget("Widget E",{{id_1,1},{id_2,1}},BASE_PRODUCTION_TIME*1.3);
		CreateWidget("Widget F",{{id_1,1},{id_2,2}},BASE_PRODUCTION_TIME*1.3);
	}
	std::unordered_map<int,Widget> widgets_;
	int next_id_;
};

#endif
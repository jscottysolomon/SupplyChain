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

#define BASE_PRODUCTION_TIME 100
#define FIRST_ID 1

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
		next_id_ = FIRST_ID;
		SetUp();
	}

	int CreateWidget(std::string name) {
		return CreateWidget(name, {}, 1, BASE_PRODUCTION_TIME);
	}

	int CreateWidget(std::string name, std::unordered_map<int,int> inputs, int output, float time) {
		Widget w = {next_id_++,1,name,inputs,output,time};
		widgets_[w.id] = w;
		return w.id;
	}

	int CreateWidget(std::string name, std::unordered_map<int,int> inputs, float time) {
		return CreateWidget(name, inputs, 1, time);
	}

	int CreateWidget(std::string name, int id, float time) {
		std::unordered_map<int,int> inputs = {{id,1}};
		return CreateWidget(name, inputs, 1, time);
	}

	int CreateWidget(std::string name, float time) {
		return CreateWidget(name, {}, 1, time);
	}

	Widget GetWidget(int id) {
		//TODO: Check if widget exists
		return widgets_[id];
	}

	void SetReceipe(int id, std::unordered_map<int,int> recipe) {
		widgets_[id].inputs = recipe;
	}

	std::unordered_map<int,int> GetRecipe(int id) {
		return widgets_[id].inputs;
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

	std::string GetWidgetName(int id) {
		if(widgets_.find(id) != widgets_.end()) {
			return widgets_.at(id).name;
		}
		
		return "Invalid";
	}

	// static std::string GetNameStatic(int id) {
	// 	if (obj == nullptr)
  //     obj = new ReceipeOrganizer();
	// 	return obj->GetWidgetName(id);
	// }

	bool ProduceWidget(Inventory* inv, int id) {
		// bool satisfied = true;
		
		// //Checking inventory has required amount for widgets
		// for(std::pair<int, int> p: widgets_[id].inputs) {
		// 	if(inv->GetWidgetQuantity(p.first) < p.second) {
		// 		satisfied = false;
		// 	}
		// }

		// if(!satisfied) return satisfied;

		// for(std::pair<int, int> p: widgets_[id].inputs) {
		// 	inv->RemoveWidget(p.first,p.second);
		// }

		// inv->AddWidget(id,widgets_[id].output_amount);

		// return satisfied;
		return false;
	}

private:
  static ReceipeOrganizer* obj;
	std::unordered_map<int,Widget> widgets_;
	int next_id_;
	void SetUp() {
		// int id_1 = CreateWidget("Widget A");
		// int id_2 = CreateWidget("Widget B");
		// CreateWidget("Widget C",{{id_1,2}},BASE_PRODUCTION_TIME*1.1);
		// CreateWidget("Widget D",{{id_2,2}},BASE_PRODUCTION_TIME*1.2);
		// CreateWidget("Widget E",{{id_1,1},{id_2,1}},BASE_PRODUCTION_TIME*1.3);
		// CreateWidget("Widget F",{{id_1,1},{id_2,2}},BASE_PRODUCTION_TIME*1.3);

		/*Mining*/
		// int iron_ore = CreateWidget("Iron Ore");
		int copper_ore = CreateWidget("Copper Ore");
		int coal = CreateWidget("Coal");

		/*Metal Processing*/
		int iron_plate = CreateWidget("Iron Plate");
		int steel_coil = CreateWidget("Steel Coil",{{iron_plate,2}, {coal,1}},500);
		int steel_bar = CreateWidget("Steel Bar",{{iron_plate,2}, {coal,1}},200);
		int bolts = CreateWidget("Bolts",{{iron_plate,2}},50);
		int screws = CreateWidget("Screws",{{iron_plate,2}},50);
		int copper_plating = CreateWidget("Copper Plating",{{copper_ore,1}}, BASE_PRODUCTION_TIME*10);

		/*Chemicals */
		int plastic = CreateWidget("Plastic",BASE_PRODUCTION_TIME*5);
		int silicon = CreateWidget("Silicon",BASE_PRODUCTION_TIME*6);
		int polyester = CreateWidget("Polyester",plastic,BASE_PRODUCTION_TIME*10);
		int computer_chip = CreateWidget("Computer Chip",{{copper_plating,3}, {plastic,1}, {silicon,1}},BASE_PRODUCTION_TIME*4);
		int adhesive = CreateWidget("Adhesive",BASE_PRODUCTION_TIME,10);

		/*Agriculture*/
		int cotton = CreateWidget("Cotton Spool",BASE_PRODUCTION_TIME*9);
		int lumber = CreateWidget("Lumber",BASE_PRODUCTION_TIME,BASE_PRODUCTION_TIME*20);
		int cotton_fabric = CreateWidget("Cotton Fabric", cotton,BASE_PRODUCTION_TIME*9);

		/*Furniture*/
		int wooden_plank = CreateWidget("Wooden Plank", lumber,BASE_PRODUCTION_TIME*4);
		int wooden_chair = CreateWidget("Wooden Chair",{{adhesive,1}, {wooden_plank, 3}, 
			{screws, 4}}, BASE_PRODUCTION_TIME*10);
		int comfortable_chair = CreateWidget("Comfortable Chair",{{cotton_fabric, 1}, {adhesive,1}, 
			{wooden_plank, 3}, {screws, 4}}, BASE_PRODUCTION_TIME*10);
	}
};

#endif
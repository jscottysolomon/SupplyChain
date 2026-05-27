/*
widget.hpp
*/

#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <set>

#include <graaflib/graph.h>

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

class Inventory {
	private:
		std::unordered_map<int, int> map_; //id,quantity
		std::set<int> whitelist_; //whitelist is default
		std::set<int> blacklist_;
		int total_capacity_;
		int widget_quantity_;

	public:
		Inventory() {
			total_capacity_ = 500;
			widget_quantity_ = 0;
		}

		//whitelist is default behavior
		bool IsAllowed(int id) {
			if(!whitelist_.empty()) {
				return whitelist_.count(id) > 0;
			} else if(!blacklist_.empty()) {
				return blacklist_.count(id) <= 0;
			}

			return true;
		}

		int AddWidget(int id, int quantity) {
			if(id < 0) return 0;
			if(quantity <= 0) return 0;
			if(widget_quantity_ == total_capacity_) return 0;
			if(!IsAllowed(id)) return 0;

			if(total_capacity_ < widget_quantity_ + quantity)  {
				quantity = (total_capacity_-widget_quantity_);	
			}
			map_[id] += quantity;
			widget_quantity_ += quantity;
			return quantity;
		}

		bool AddWidget(int id) {
			return AddWidget(id, 1);
		}

		int RemoveWidget(int id, int quantity) {
			if(quantity <= 0) return 0;
			int current_quantity = map_[id];
			if(current_quantity < quantity) {
				quantity = current_quantity;
			}
			map_[id] -= quantity;
			widget_quantity_ += quantity;
			return quantity;
		}

		bool RemoveWidget(int id) {
			return RemoveWidget(id,1);
		}

		int GetWidgetQuantity(int id) {
			return map_[id];
		}

		int GetNextWidget() {
			for(std::pair<int,int> p: map_) {
				if(RemoveWidget(p.first)) {
					return p.first;
				}
			}

			return -1;
		}

		int GetNextWhitelist(std::set<int> whitelist) {
			for(std::pair<int,int> p: map_) {
				if(whitelist.count(p.first) && RemoveWidget(p.first)) {
					return p.first;
				}
			}

			return -1;
		}

		int GetNextBlacklist(std::set<int> lst) {
			for(std::pair<int,int> p: map_) {
				if(!lst.count(p.first) && RemoveWidget(p.first)) {
					return p.first;
				}
			}

			return -1;
		}

		std::set<int> GetWhitelist() {
			return whitelist_;
		}

		std::set<int> GetBlacklist() {
			return blacklist_;
		}

		void AddBlacklist(int id) {
			if(whitelist_.size() > 0) return;

			blacklist_.insert(id);
		}

		void SetWhitelist(std::set<int> lst) {
			whitelist_ = lst;

			ClearBlacklist();
		}

		void SetBlacklist(std::set<int> lst) {
			blacklist_ = lst;

			ClearWhitelist();
		}

		void AddWhitelist(int id) {
			whitelist_.insert(id);
		}

		void ClearWhitelist() {
			whitelist_.clear();
		}

		void ClearBlacklist() {
			blacklist_.clear();
		}

		void ClearAllowable() {
			ClearWhitelist();
			ClearBlacklist();
		}

		bool UsesWhitelist() {
			return blacklist_.size() <= 0;
		}

		bool IsFull() {
			return total_capacity_ == widget_quantity_;
		}

		int GetAvailableCapacity() {
			return total_capacity_ - widget_quantity_;
		}

		void SetInventory(std::unordered_map<int, int> inv) {
			map_ = inv;
		}

		std::unordered_map<int, int> GetInventory() {
			return map_;
		}

		int RequestWidget(int id, int amt) {
			if(amt <= 0) return 0;

			//TODO logic of how much to request (i.e reservations)

			return RemoveWidget(id,amt);
		}

		int RequestGiveWidget(int id, int amt) {
			//TODO logic of how much willing to accept

			return AddWidget(id,amt);
		}
		
		//Truck says here's what i have and factory decides what it wants and how much it takes per tick
};

#endif
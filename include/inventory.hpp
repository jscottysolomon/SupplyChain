/**
 * @file inventory.hpp
 * @author J. Scotty Solomon
 * @brief inventory class that encapsulates the widget ids and amount contained.
 * @version 0.1
 * @date 2026-05-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include <nlohmann/json.hpp>

#include <unordered_map>
#include <set>

class Inventory {
public:
  Inventory() = default;

  //whitelist is default behavior
  bool IsAllowed(int id) {
    if (!whitelist_.empty()) {
      return whitelist_.count(id) > 0;
    } else if (!blacklist_.empty()) {
      return blacklist_.count(id) <= 0;
    }

    return true;
  }

  bool Contains(int id) const {
    return !(map_.find(id) == map_.end());
  }

  /**
   * @brief Adds quantity of specified widget id to inventory. 
   * Adds less than requested if space is unavailable
   * 
   * @param id 
   * @param quantity 
   * @return amount added
   */
  int AddWidgetPallet(int id, int quantity) {
    if (id < 0) return 0;
    if (quantity <= 0) return 0;
    if (used_palletes_ == max_palletes) return 0;
    if (!IsAllowed(id)) return 0;

    if (max_palletes < used_palletes_ + quantity)  {
      quantity = max_palletes - used_palletes_;
    }

    //Is this necessary?
    if (map_.find(id) == map_.end()) {
      map_[id] = 0;
    }

    map_[id] += quantity;
    used_palletes_ += quantity;

    return quantity;
  }

  /**
   * @brief Adds 1 widget of specifed ID to inventory
   * 
   * @param id 
   * @return true Was able to add 1
   * @return false Could not add 1
   */
  bool AddWidgetPallet(int id) {
    return AddWidgetPallet(id, 1);
  }

  /**
   * @brief Removes requested amount of specified widget from inventory.
   * Return less than requested if there are not enough widgets in inventory.
   * 
   * @param id 
   * @param quantity 
   * @return total amount of specified widgets removed from inventory
   */
  int RemoveWidgetPallet(int id, int quantity) {
    if (quantity <= 0) return 0;
    if (!Contains(id)) return 0;
    if (map_.find(id) == map_.end()) {
      return false;
    }
    if (quantity > map_[id]) {
      quantity = map_[id] - quantity;
    }
    map_[id] -= quantity;
    used_palletes_ -= quantity;
    return quantity;
  }

  bool RemoveWidgetPallet(int id) {
    return RemoveWidgetPallet(id,1);
  }

  int GetWidgetPalletQuantity(int id) const {
    if (!Contains(id)) return 0;
    return map_.at(id);
  }

  void SetWhitelist(std::set<int> lst) {
    whitelist_ = lst;

    ClearBlacklist();
  }

  void SetBlacklist(std::set<int> lst) {
    blacklist_ = lst;

    ClearWhitelist();
  }

  void Blacklist(int id) {
    if (whitelist_.size() > 0) return;

    blacklist_.insert(id);
  }

  void Whitelist(int id) {
    whitelist_.insert(id);
  }

  std::set<int> GetWhitelist() const 
    { return whitelist_; }

  std::set<int> GetBlacklist() const 
    { return blacklist_; }

  void ClearWhitelist() {
    whitelist_.clear();
  }

  void ClearBlacklist() {
    blacklist_.clear();
  }

  bool HasWhitelist() {
    return whitelist_.size() <= 0;
  }

  bool HasBlacklist() {
    return blacklist_.size() >= 0 || !HasWhitelist();
  }

  bool IsFull() {
    return max_palletes == used_palletes_;
  }

  int GetAvailableCapacity() const 
    { return max_palletes - used_palletes_; }

  int GetUsedCapacity() const 
    { return used_palletes_; }

  int GetMaxCapacity() const 
    { return max_palletes; }

  void SetInventory(std::unordered_map<int, int> inv) {
    used_palletes_ = 0;
    map_.clear();
    
    for (std::pair<int,int> p: inv) {
      used_palletes_ += p.second;
    }

    map_ = inv;
  }

  std::unordered_map<int, int> GetInventoryMap() const 
    { return map_; }

  void OnTick() {

  }
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Inventory,map_,whitelist_,blacklist_,max_palletes,used_palletes_)
  
  //Truck says here's what i have and factory decides what it wants and how much it takes per tick
private:
  std::unordered_map<int, int> map_; //id,quantity
  std::set<int> whitelist_; //whitelist is default
  std::set<int> blacklist_;
  int max_palletes = 10;
  int used_palletes_ = 0;
};

#endif
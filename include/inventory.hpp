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

  /**
   * @brief If inventory has pallet of given type
   * 
   * @param id 
   * @return true 
   * @return false 
   */
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
  int AddPallet(int widget_id, int quantity) {
    if (widget_id < 0) return 0;
    if (quantity <= 0) return 0;
    if (used_palletes_ == max_palletes_) return 0;
    if (!IsAllowed(widget_id)) return 0;

    if (max_palletes_ < used_palletes_ + quantity)  {
      quantity = max_palletes_ - used_palletes_;
    }

    //Is this necessary?
    if (map_.find(widget_id) == map_.end()) {
      map_.at(widget_id) = 0;
    }

    map_.at(widget_id) += quantity;
    used_palletes_ += quantity;

    return quantity;
  }

  /**
   * @brief Adds 1 pallet of specifed widget to inventory
   * 
   * @param id 
   * @return true Was able to add 1
   * @return false Could not add 1
   */
  bool AddPallet(int widget_id) {
    return AddPallet(widget_id, 1);
  }

  /**
   * @brief Removes requested amount of specified widget from inventory.
   * Return less than requested if there are not enough widgets in inventory.
   * 
   * @param id 
   * @param quantity 
   * @return total amount of specified widgets removed from inventory
   */
  int RemovePallet(int id, int quantity) {
    if (quantity <= 0) return 0;
    if (!Contains(id)) return 0;
    if (map_.find(id) == map_.end()) {
      return false;
    }
    if (quantity > map_.at(id)) {
      quantity = map_.at(id)- quantity;
    }
    map_.at(id)-= quantity;
    used_palletes_ -= quantity;
    return quantity;
  }

  bool RemovePallet(int id) {
    return RemovePallet(id,1);
  }

  /**
   * @brief Number of pallets of widget type
   * 
   * @param id 
   * @return int 
   */
  int GetPalletQuantity(int id) const {
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

  /**
   * @brief If Inventory is full
   * 
   * @return true 
   * @return false 
   */
  bool IsFull() const
    { return max_palletes_ == used_palletes_; }

  /**
   * @brief Amount of pallets inventory has space for
   * 
   * @return int 
   */
  int GetAvailableCapacity() const 
    { return max_palletes_ - used_palletes_; }

  /**
   * @brief Amount of pallets in inventory
   * 
   * @return int 
   */
  int GetUsedCapacity() const 
    { return used_palletes_; }

  /**
   * @brief Maximum amount of pallets Inventory can hold.
   * 
   * @return int 
   */
  int GetMaxCapacity() const 
    { return max_palletes_; }

  void SetMaxCapacity(int max) 
    {int max_palletes_ = max;}

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
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Inventory,map_,whitelist_,blacklist_,max_palletes_,used_palletes_)
  
  //Truck says here's what i have and factory decides what it wants and how much it takes per tick
private:
  std::unordered_map<int, int> map_; //id,quantity
  std::set<int> whitelist_; //whitelist is default
  std::set<int> blacklist_;
  int max_palletes_ = 10;
  int used_palletes_ = 0;
};

#endif
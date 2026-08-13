#ifndef STRATEGY_HPP
#define STARTEGY_HPP

#include <set>
#include <time.h>
#include <unordered_map>

#include "inventory.hpp"
#include "util.hpp"
#include "widget.hpp"


////////////////////////////////////////////////////////
//Strategy Base Class
////////////////////////////////////////////////////////
class Strategy {
public:
    Strategy() {
        receiver_inventory_ = nullptr;
        unloader_inventory_ = nullptr;
        whitelist_ = nullptr;
        blacklist_ = nullptr;
    }
    ~Strategy() {}
    void SetReceiverInventory(Inventory* rec) {
        receiver_inventory_ = rec;
    }
    void SetUnloaderInventory(Inventory* unl) {
        unloader_inventory_ = unl;
    }
    void SetInventory(Inventory* rec, Inventory* unl) {
        receiver_inventory_ = rec;
        unloader_inventory_ = unl;
    }
    void SetLists(std::set<int>* wlist, std::set<int>* blist) {
        whitelist_ = wlist;
        blacklist_ = blist;
    }
    std::set<int>* GetWhitelist() {return whitelist_;}
    std::set<int>* GetBlacklist() {return blacklist_;}
    virtual bool Run() {
        return false;
    }
    virtual bool Run(int) {
        return false;
    }
protected:
    Inventory* receiver_inventory_;
    Inventory* unloader_inventory_;
    std::set<int>* whitelist_;
    std::set<int>* blacklist_;
};


#endif
#ifndef LOAD_PLAN_HPP
#define LOAD_PLAN_HPP

#include <set>
#include <time.h>
#include <unordered_map>

#include "rules.hpp"
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


////////////////////////////////////////////////////////
//Widget Strategy
////////////////////////////////////////////////////////
class WidgetStrategy : public Strategy {
public: 
    WidgetStrategy(int amt) {
        amount_ = amt;
        loaded_amount_ = 0;
    }
protected: 
    int amount_;
    int loaded_amount_;
    bool Ready() {

    }
};

class ExactQuantityStrategy : public WidgetStrategy {
public:
    ExactQuantityStrategy(int amount) : WidgetStrategy(amount) {}
    virtual bool Run(int id) override {
        if(unloader_inventory_ == nullptr 
            || receiver_inventory_ == nullptr) {return false;}
        
        if(receiver_inventory_->GetAvailableCapacity() <= 0) 
            return false;

        if(!receiver_inventory_->IsAllowed(id)) {return false;}

        if(unloader_inventory_->RemoveWidget(id)) {
            receiver_inventory_->AddWidget(id);
        }
        loaded_amount_ += 1;

        if(loaded_amount_ >= amount_) {
            if(whitelist_ != nullptr && !whitelist_->empty() && whitelist_->count(id)) {
                whitelist_->erase(id);
            //Does it matter if blacklist is empty? Should I just add it anyway?
            } else if(blacklist_ != nullptr && !blacklist_->empty() && !blacklist_->count(id)) {
                blacklist_->insert(id);
            }

            return true;
        }

        return false;
    }
};

class MinimumQuantityStrategy : public WidgetStrategy {
public:
    MinimumQuantityStrategy(int amount) : WidgetStrategy(amount) {}
    virtual bool Run(int id) override {
        if(unloader_inventory_ == nullptr 
            || receiver_inventory_ == nullptr) {return false;}
        
        if(receiver_inventory_->GetAvailableCapacity() <= 0) 
            return false;

        if(!receiver_inventory_->IsAllowed(id)) {return false;}

        if(unloader_inventory_->RemoveWidget(id)) {
            receiver_inventory_->AddWidget(id);
        }
        loaded_amount_ += 1;

        if(loaded_amount_ >= amount_) {
            return true;
        }

        return false;
    }
};

// class LoadAtMax : WidgetStrategy {
// public:
//     LoadAtMax(int amount) {
//         amount_ = amount;
//         loaded_amount_ = 0;
//     }
//     bool LoadWidget(LoadPlan* plan, int id) override {
//         plan->unloader_inventory->AddWidget(id,1);
//         plan->receiver_inventory->RemoveWidget(id,1);
//         loaded_amount_ += 1;

//         if(!plan->whitelist.empty() && plan->whitelist.count(id)) {
//             plan->whitelist.erase(id);
//         } else if(!plan->blacklist.empty() && !plan->blacklist.count(id)) {
//             plan->blacklist.insert(id);
//         }

//         return loaded_amount_ >= amount_;
//     }
// private:
//     int amount_;
//     int loaded_amount_;
// };

class TertieryStrategy : public Strategy {};

class LoadListStrategy : public TertieryStrategy {
    virtual bool Run() override {
        if(unloader_inventory_ == nullptr 
            || receiver_inventory_ == nullptr) {return false;}

        if(receiver_inventory_->IsFull()) {return false;}

        bool loaded = false;
        
        if(whitelist_ != nullptr && !whitelist_->empty() ){
            for(int id: *whitelist_) {
                if(receiver_inventory_->IsAllowed(id) && 
                    unloader_inventory_->RemoveWidget(id)) {
                        receiver_inventory_->AddWidget(id);
                        loaded = true;
                        break;
                }
            }
        } else if(blacklist_ != nullptr) {
            for(std::pair<int,int> p  :unloader_inventory_->GetInventory()) {
                if(!blacklist_->count(p.first) && unloader_inventory_->RemoveWidget(p.first)) {
                    receiver_inventory_->AddWidget(p.first);
                    loaded = true;
                    break;
                }
            }
        }

        return false;

        // return !loaded; //should this func ever return true? this is the last thing to load
    }
};

////////////////////////////////////////////////////////
//Finish Strategy
////////////////////////////////////////////////////////

class FinishStrategy : public Strategy{};

class AtCapacityStrategy : public FinishStrategy {
public:
    virtual bool Run() override {
        if(receiver_inventory_ == nullptr) return false;
        return receiver_inventory_->IsFull();
    }
};

class ToCapacityStrategy : public FinishStrategy {
public: 
    ToCapacityStrategy(int capacity) {
        capacity_ = capacity;
    }
    virtual bool Run() override {
        if(receiver_inventory_ == nullptr) return false;
        return receiver_inventory_->GetAvailableCapacity() == capacity_;
    }
private:
    int capacity_;
};

class LoadPlanner {
public: 
    LoadPlanner() {
        finished_ = false;
        unloader_agreed_ = false;
        tertiary_strategy_ = nullptr;
        finish_strategy_ = nullptr;
    }
    ~LoadPlanner() {
        // if(tertiary_strategy_ != nullptr) {
        //     delete tertiary_strategy_;
        //     tertiary_strategy_ = nullptr;
        // }
        // if(finish_strategy_ != nullptr) {
        //     delete finish_strategy_;
        //     finish_strategy_ = nullptr;
        // }
    }
    //TODO deconstructor
    void SetReceiverInventory(Inventory* inv) {
        receiver_inventory_ = inv;
    }
    std::unordered_map<int,int> GetReceiverInventory() {
        return receiver_inventory_->GetInventory();
    }
    void SetUnloaderInventory(Inventory* inv) {
        unloader_inventory_ = inv;
    }
    std::unordered_map<int,int> GetUnloaderInventory() {
        return unloader_inventory_->GetInventory();
    }
    void SetWhitelist(std::set<int> lst) {
        whitelist_ = lst;
    }
    std::set<int> GetWhitelist() {return whitelist_;}
    void SetBlacklist(std::set<int> lst) {
        blacklist_ = lst;
    }
    std::set<int> GetBlacklist() {return blacklist_;}
    clock_t GetLastLoad() {return last_load_;}
    void SetSpeed(float speed) {speed_ = speed;}
    float GetSpeed(){return speed_;}
    void UnloaderAgreed() {unloader_agreed_ = true;}
    bool IsFinished() {return finished_;}

    void AddWidgetStrategy(WidgetStrategy* strat, int id) {
        strat->SetInventory(receiver_inventory_,unloader_inventory_);
        strat->SetLists(&whitelist_,&blacklist_);
        widgets_.insert({id,strat});
    }

    void AddTertieryStrategy(TertieryStrategy* strat) {
        strat->SetInventory(receiver_inventory_,unloader_inventory_);
        strat->SetLists(&whitelist_,&blacklist_);
        tertiary_strategy_ = strat;
    }

    void AddFinisherStrategy(FinishStrategy* strat) {
        strat->SetInventory(receiver_inventory_,unloader_inventory_);
        strat->SetLists(&whitelist_,&blacklist_);
        finish_strategy_ = strat;
    }

    void Load();

private:
    Inventory* receiver_inventory_;
    Inventory* unloader_inventory_;
	std::set<int> whitelist_;
	std::set<int> blacklist_;
    std::unordered_map<int,WidgetStrategy*> widgets_;
    FinishStrategy* finish_strategy_;
    TertieryStrategy* tertiary_strategy_;
    bool finished_;
	float speed_;	//how fast you can load widget
    clock_t last_load_;
    bool unloader_agreed_;
};

#endif
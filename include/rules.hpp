#ifndef RULES_HPP
#define RULES_HPP


#include <unordered_map>
#include <vector>
#include <queue>
#include <time.h>

#include "util.hpp"
#include "widget.hpp"

class LoadUntil;
class WidgetPlan;
class LoadTertiary;

struct LoadPlan {
    Inventory* receiver_inventory;
    Inventory* unloader_inventory;
	std::set<int> whitelist;
	std::set<int> blacklist;
    std::unordered_map<int,WidgetPlan*> widgets;
    LoadUntil* finish_plan;
    LoadTertiary* load_tertiary;
    bool finished;
	float speed;	//how fast you can load widget
    clock_t last_load;
};

void Load(LoadPlan* plan);

class LoadUntil {
public:
    virtual void LoadIsFinished(LoadPlan* plan);
};

class LoadUntilFull : public LoadUntil {
public:
    virtual void LoadIsFinished(LoadPlan* plan) override {
        plan->finished = plan->unloader_inventory->IsFull();
    }
};

class LoadUntilCapacity : public LoadUntil {
public:
    LoadUntilCapacity(int until) {
        load_until_ = until;
    }
    virtual void LoadIsFinished(LoadPlan* plan) override {
        plan->finished = plan->unloader_inventory->GetAvailableCapacity() <= load_until_;
    }
private:
    int load_until_;
};

class LoadUntilLowestPriority : public LoadUntil {
public:
    void virtual LoadIsFinished(LoadPlan* plan) override {
        plan->finished = plan->widgets.empty();
    }
};

class WidgetPlan {
public:
    bool virtual LoadWidget(LoadPlan* plan, int id);
};

class LoadExactly : WidgetPlan {
public:
    LoadExactly(int amount) {
        amount_ = amount;
        loaded_amount_ = 0;
    }
    bool LoadWidget(LoadPlan* plan, int id) override {
        plan->unloader_inventory->AddWidget(id,1);
        plan->receiver_inventory->RemoveWidget(id,1);
        loaded_amount_ += 1;

        if(!plan->whitelist.empty() && plan->whitelist.count(id)) {
            plan->whitelist.erase(id);
        } else if(!plan->blacklist.empty() && !plan->blacklist.count(id)) {
            plan->blacklist.insert(id);
        }

        return loaded_amount_ >= amount_;
    }
private:
    int amount_;
    int loaded_amount_;
};

class LoadAtleast : WidgetPlan {
public:
    LoadAtleast(int amount) {
        amount_ = amount;
        loaded_amount_ = 0;
    }
    bool LoadWidget(LoadPlan* plan, int id) override {
        plan->unloader_inventory->AddWidget(id,1);
        plan->receiver_inventory->RemoveWidget(id,1);
        loaded_amount_ += 1;

        return loaded_amount_ >= amount_;
    }
private:
    int amount_;
    int loaded_amount_;
};

class LoadAtMax : WidgetPlan {
public:
    LoadAtMax(int amount) {
        amount_ = amount;
        loaded_amount_ = 0;
    }
    bool LoadWidget(LoadPlan* plan, int id) override {
        plan->unloader_inventory->AddWidget(id,1);
        plan->receiver_inventory->RemoveWidget(id,1);
        loaded_amount_ += 1;

        if(!plan->whitelist.empty() && plan->whitelist.count(id)) {
            plan->whitelist.erase(id);
        } else if(!plan->blacklist.empty() && !plan->blacklist.count(id)) {
            plan->blacklist.insert(id);
        }

        return loaded_amount_ >= amount_;
    }
private:
    int amount_;
    int loaded_amount_;
};

class LoadTertiary {
public:
    virtual void LoadLowestPrioirty(LoadPlan* plan);
};

class LoadWhitelist: LoadTertiary {
public:
    virtual void LoadLowestPrioirty(LoadPlan* plan) override {
        if(plan->whitelist.empty()) return;
        for(int id: plan->whitelist) {
            if(plan->receiver_inventory->GetWidgetQuantity(id) > 0) {
                plan->unloader_inventory->AddWidget(id,1);
                plan->receiver_inventory->RemoveWidget(id,1);
                return;
            }
        }
    }
};

class LoadBlacklist: LoadTertiary {
public: 
    virtual void LoadLowestPrioirty(LoadPlan* plan) override {
        plan->unloader_inventory->AddWidget(
            plan->receiver_inventory->GetNextBlacklist(plan->blacklist));
    }
};

#endif
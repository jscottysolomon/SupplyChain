#include "load_plan.hpp"

#include <set>
#include <time.h>
#include <unordered_map>

#include "util.hpp"
#include "widget.hpp"

void LoadPlan::Load() {
    if(receiver_inventory_ == nullptr 
        || unloader_inventory_ == nullptr) {return;}

    if(finished_) {
        return;
    }

    if((GetGlobalTime() - last_load_ ) / CLOCKS_PER_SEC < speed_)
        return;

    int current_capacity = receiver_inventory_->GetAvailableCapacity();
    bool remove = false;
    int remove_id = -1;
    bool loaded = false;

    for(std::pair<int,WidgetStrategy*> p: widgets_) {
        if(p.second == nullptr) continue;
        remove = p.second->Run(p.first);
        if(current_capacity != receiver_inventory_->GetAvailableCapacity()) {
            loaded = true;
            remove_id = p.first;
            break;
        } else {
            continue;
        }
    }

    if(remove) {
        //TODO fix memory leak;
        if(widgets_.find(remove_id) != widgets_.end()) {
            WidgetStrategy* strat = widgets_.at(remove_id);
            widgets_.erase(remove_id);
            delete strat;
        } else {
            return;
        }
    }

    if(tertiary_strategy_ != nullptr && !loaded) {
        remove = tertiary_strategy_->Run();
        loaded = current_capacity != receiver_inventory_->GetAvailableCapacity();
        if(remove) {
            delete tertiary_strategy_;
            tertiary_strategy_ = nullptr;
            
        }
    }

    if(loaded) {
        last_load_ = GetGlobalTime();
    }

    if(finish_strategy_ != nullptr) {
        finished_ = finish_strategy_->Run();
    }
}
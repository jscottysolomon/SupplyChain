#include "load_planner.hpp"

#include <set>
#include <time.h>
#include <unordered_map>

#include "rules.hpp"
#include "util.hpp"
#include "widget.hpp"

void LoadPlanner::Load() {
    if(receiver_inventory_ == nullptr 
        || unloader_inventory_ == nullptr) {return;}

    if((GetGlobalTime() - last_load_ ) / CLOCKS_PER_SEC < speed_)
        return;

    int current_capacity = receiver_inventory_->GetAvailableCapacity();
    bool remove = -1;
    int remove_id = -1;
    bool loaded = false;

    for(std::pair<int,WidgetStrategy*> p: widgets_) {
        remove = p.second->Run(p.first);
        if(current_capacity != receiver_inventory_->GetAvailableCapacity()) {
            loaded = true;
            remove_id = p.first;
            break;
        }
    }

    if(remove) {
        widgets_.erase(remove_id);
    }

    if(loaded) {
        last_load_ = GetGlobalTime();
        return;
    }
}
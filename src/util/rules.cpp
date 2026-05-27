#include "rules.hpp"

void Load(LoadPlan* plan) {
    if(plan->finished) return;
    if((GetGlobalTime() - plan->last_load) / CLOCKS_PER_SEC < plan->speed) return;
    plan->last_load = GetGlobalTime();

    int remove = -1;
    int prev_capacity = plan->unloader_inventory->GetAvailableCapacity();
    bool added = false;

    for(std::pair<int,WidgetPlan*> p: plan->widgets) {
        remove = p.second->LoadWidget(plan,p.first);

        if(prev_capacity > plan->unloader_inventory->GetAvailableCapacity()) {
            added = true;
            break;
        }
    }

    if(remove > 0) {
        plan->widgets.erase(remove);
    }

    if(added) {
        plan->finish_plan->LoadIsFinished(plan);
        return;
    }

    plan->load_tertiary->LoadLowestPrioirty(plan);
    // added = prev_capacity > plan->unloader_inventory->GetAvailableCapacity();
    plan->finish_plan->LoadIsFinished(plan);
}

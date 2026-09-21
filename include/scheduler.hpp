#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "rules.hpp"

/*
 - decides what the truck should do next
  - assigns stops
  - assigns plans
  - reacts when a truck arrives/docks/finishes loading
  - logic ties UI to backend
*/

class Scheduler {
public: 

  Scheduler() = default;

  Rule CreateRule(RuleType type, int widget_id, int amount, int step) {
    Rule rule;
    rule.SetAmount(amount);
    rule.SetStep(step);
    rule.SetType(type);
    rule.SetWidgetId(widget_id);
    return rule;
  }

  Action CreateAction(ActionType type, int widget_id_) {
    Action action;
    action.SetType(type);
    action.SetWidgetId(widget_id_);
    return action;
  }

  void AddTarget(int truck_id, int factory_id, Rule rule, Action action) {
    Plan plan = plans_.at(truck_id);
    plan.AddTarget(rule,action);
  }

  Plan GetPlan(int truck_id) {
    return plans_.at(truck_id);
  }

  void OnTick() {

  }

private:
  int next_plan_id_ = 0;

  std::unordered_map<int, Plan> plans_;
  std::unordered_map<int, int> truck_plan_ids_;
};

#endif 
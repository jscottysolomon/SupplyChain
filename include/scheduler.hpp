#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "rules.hpp"
#include "traffic.hpp"
#include "truck.hpp"
#include "util.hpp"
#include "factory.hpp"

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

  Scheduler(TrafficCommand* command) {
    commander_ = command;
  }

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
    std::pair<int,int> key = {truck_id,factory_id};
    Plan& plan = plans_.at(key);
    plan.AddTarget(rule,action);
  }

  void CreatePlan(Truck* truck, Factory* factory) {
    if(!truck || !factory) return;
    RuleContext context = {truck->GetInventory(), factory->GetInventory(), 
      truck->GetId(), factory->GetId()};
    Plan plan(context);
    plan.SetId(next_plan_id_++);
    std::pair<int,int> key = {truck->GetId(),factory->GetId()};
    plans_.emplace(key, plan);
    truck->AddPlan(factory->GetId(),plan.GetId());
  }

  Plan& GetPlan(int truck_id, int factory_id) {
    std::pair<int,int> key = {truck_id,factory_id};
    return plans_.at(key);
  }

  void OnTick() {

  }

  void AddToSchedule(int truck_id, int junction_id) {
    Truck* truck = commander_->GetTruck(truck_id);
    Junction* junction = commander_->GetJunction(junction_id);

    if(truck && junction) {
      truck->AddToSchedule(junction);
      
    } else {
      TraceLog(LOG_ERROR, "Cannot find truck or junction specified");
    }
  }

  void SetUp();

private:
  int next_plan_id_ = 0;

  std::unordered_map<std::pair<int,int>, Plan,PairHash> plans_; //{{truckId, factory} , Plan}

  TrafficCommand* commander_;
};

#endif 
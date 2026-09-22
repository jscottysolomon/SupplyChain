#include "logistics.hpp"

#include "factory.hpp"
#include "rules.hpp"
#include "truck.hpp"

Rule TransferPlanner::CreateRule(RuleType type, int widget_id, int amount, int step) {
  Rule rule;
  rule.SetAmount(amount);
  rule.SetStep(step);
  rule.SetType(type);
  rule.SetWidgetId(widget_id);
  return rule;
}

Action TransferPlanner::CreateAction(ActionType type, int widget_id_) {
  Action action;
  action.SetType(type);
  action.SetWidgetId(widget_id_);
  return action;
}

Plan TransferPlanner::CreatePlan(Truck* truck, Factory* factory) {
  if(!truck || !factory) {
    TraceLog(LOG_ERROR, "Truck or Factory is null!");
  }
  RuleContext context = {truck->GetInventory(), factory->GetInventory(), 
    truck->GetId(), factory->GetId()};
  Plan plan(context);
  plan.SetId(next_plan_id_++);
  std::pair<int,int> key = {truck->GetId(),factory->GetId()};
  truck->AddPlan(factory->GetId(),plan.GetId());

  return plan;
}

void LoadPlanManager::SavePlan(int truck_id, int factory_id, Plan plan) {
    std::pair key = {truck_id,factory_id};
    plans_.emplace(key,plan);
  }

Plan& LoadPlanManager::GetPlan(int truck_id, int factory_id) {
  std::pair<int,int> key = {truck_id,factory_id};
  return plans_.at(key);
}

void LoadPlanManager::AddTarget(int truck_id, int factory_id, Rule rule, Action action) {
  std::pair<int,int> key = {truck_id,factory_id};
  Plan& plan = plans_.at(key);
  plan.AddTarget(rule,action);
}

void LoadPlanManager::OnTick() {
  for(std::pair<std::pair<int,int>,Plan> pair : plans_) {
    int truck_id = pair.first.first;
    int factory_id = pair.first.second;
    Plan& plan = pair.second;
  }
}

void RouteScheduler::AddToSchedule(int truck_id, int junction_id) {
  if(!commander_) return;

  Truck* truck = commander_->GetTruck(truck_id);
  Junction* junction = commander_->GetJunction(junction_id);

  if(truck && junction) {
    truck->AddToSchedule(junction);
    
  } else {
    TraceLog(LOG_ERROR, "Cannot find truck or junction specified");
  }
}
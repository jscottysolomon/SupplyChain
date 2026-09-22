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

/**
 * @brief Decides or creates the agreement between truck and factory: what moves, amounts, rules
 * 
 */
class TransferPlanner {
public: 
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

  Plan CreatePlan(Truck* truck, Factory* factory) {
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

  private: 
  int next_plan_id_ = 0;

};

/**
 * @brief Stores, retrieves, updates, and maybe executes Plans
 * 
 */
class LoadPlanManager {
  public: 
  void SavePlan(int truck_id, int factory_id, Plan plan) {
    std::pair key = {truck_id,factory_id};
    plans_.emplace(key,plan);
  }

  Plan& GetPlan(int truck_id, int factory_id) {
    std::pair<int,int> key = {truck_id,factory_id};
    return plans_.at(key);
  }

  void AddTarget(int truck_id, int factory_id, Rule rule, Action action) {
    std::pair<int,int> key = {truck_id,factory_id};
    Plan& plan = plans_.at(key);
    plan.AddTarget(rule,action);
  }

  void OnTick() {
    for(std::pair<std::pair<int,int>,Plan> pair : plans_) {
      int truck_id = pair.first.first;
      int factory_id = pair.first.second;
      Plan& plan = pair.second;

    }
  }

  private:
  std::unordered_map<std::pair<int,int>, Plan, PairHash> plans_;
};

/**
 * @brief Assigns stops/junctions to trucks
 * 
 */
class RouteScheduler {
public: 

  RouteScheduler() = default;

  RouteScheduler(TrafficCommand* command) {
    commander_ = command;
  }

  void OnTick() {

  }

  void SetCommander(TrafficCommand* commander) {
    commander_ = commander;
  }

  void AddToSchedule(int truck_id, int junction_id) {
    if(!commander_) return;

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
  TrafficCommand* commander_ = nullptr;
};

/**
 * @brief Layer between UI and Backend. Each function shouldn't be more
 * than 2-3 lines of code.
 * 
 */
class LogisticsController {
public:  

  LogisticsController(TrafficCommand* commander) {
    routeScheduler_.SetCommander(commander);
  }

  void SetCommander(TrafficCommand* commander) {
    routeScheduler_.SetCommander(commander);
  }

  Plan& GetPlan(int truck_id, int factory_id) 
    {return planManager_.GetPlan(truck_id,factory_id);}

  void AddStopToTruck(int truck_id, int junction_id) {
    routeScheduler_.AddToSchedule(truck_id,junction_id);
  }

  void RemoveStopFromTruck(int truck_id, int junction_id);

  Action CreateAction(ActionType type, int widget_id_)
    {return transferPlanner_.CreateAction(type,widget_id_);}

  void CreatePlan(Truck* truck, Factory* factory) {
    Plan plan = transferPlanner_.CreatePlan(truck,factory);
    planManager_.SavePlan(truck->GetId(),factory->GetId(),plan);
  }
  
  void AddPlanTarget(int truck_id, int factory_id, RuleType rule_type, int widget_id, int amount, int step, ActionType action_type) {
    Rule rule = transferPlanner_.CreateRule(rule_type,widget_id,amount,step);
    Action action = transferPlanner_.CreateAction(action_type,widget_id);
    planManager_.AddTarget(truck_id,factory_id,rule,action);
  }

  void OnTruckArrived(int truck_id, int factory_id);
  void OnTruckDocked(int truck_id, int factory_id);
  void OnLoadingFinished(int truck_id, int factory_id);

  void OnTick();

private:
  RouteScheduler routeScheduler_;
  TransferPlanner transferPlanner_;
  LoadPlanManager planManager_;
};

#endif 
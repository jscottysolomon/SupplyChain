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
  Rule CreateRule(RuleType type, int widget_id, int amount, int step);

  Action CreateAction(ActionType type, int widget_id_);

  Plan CreatePlan(Truck* truck, Factory* factory);

  private: 
  int next_plan_id_ = 0;

};

/**
 * @brief Stores, retrieves, updates, and maybe executes Plans
 * 
 */
class LoadPlanManager {
  public: 
  void SavePlan(int truck_id, int factory_id, Plan plan);

  Plan& GetPlan(int truck_id, int factory_id);

  void AddTarget(int truck_id, int factory_id, Rule rule, Action action);

  void OnTick();

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

  RouteScheduler(TrafficCommand* command) 
    { commander_ = command; }

  void SetCommander(TrafficCommand* commander) {
    commander_ = commander;
  }

  void AddToSchedule(int truck_id, int junction_id);

  void SetUp();

private:
  TrafficCommand* commander_ = nullptr;
};

/**
 * @brief Layer between UI and Backend. Each function shouldn't be more
 * than 2-3 lines of code.
 * 
 */
class LogisticsController : public TrafficEventListener{
public:  

  LogisticsController(TrafficCommand* commander) 
    { routeScheduler_.SetCommander(commander); }

  void SetCommander(TrafficCommand* commander) 
    { routeScheduler_.SetCommander(commander); }

  Plan& GetPlan(int truck_id, int factory_id) 
    {return planManager_.GetPlan(truck_id,factory_id);}

  void AddStopToTruck(int truck_id, int junction_id) 
    { routeScheduler_.AddToSchedule(truck_id,junction_id); }

  void RemoveStopFromTruck(int truck_id, int junction_id);

  Action CreateAction(ActionType type, int widget_id_)
    {return transferPlanner_.CreateAction(type,widget_id_); }

  void CreatePlan(Truck* truck, Factory* factory) {
    Plan plan = transferPlanner_.CreatePlan(truck,factory);
    planManager_.SavePlan(truck->GetId(),factory->GetId(),plan);
  }
  
  void AddPlanTarget(int truck_id, int factory_id, RuleType rule_type, int widget_id, int amount, int step, ActionType action_type) {
    Rule rule = transferPlanner_.CreateRule(rule_type,widget_id,amount,step);
    Action action = transferPlanner_.CreateAction(action_type,widget_id);
    planManager_.AddTarget(truck_id,factory_id,rule,action);
  }

  void OnTruckDocked(int truck_id, int factory_id, int dock_id) override {
    // handle load/unload
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
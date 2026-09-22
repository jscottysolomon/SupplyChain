#ifndef RULES_HPP
#define RULES_HPP

#include <algorithm>
#include <set>
#include <vector>

#include <nlohmann/json.hpp>

#include "widget.hpp"
#include "inventory.hpp"


enum RuleType {
  kReceiveWidgetPalletQuantity = 0,
  kReceivePalletQuantity,
  kDispatchWidgetPalletQuantity,
  kDispatchPalletQuantity,
  kFactoryIsFull,
  kTruckIsFull,
  kInvalidRule
};

NLOHMANN_JSON_SERIALIZE_ENUM( RuleType, {
  {kReceiveWidgetPalletQuantity, "receiveWidgetPalletQuantity"},
  {kReceivePalletQuantity, "receivePalletQuantity"},
  {kDispatchWidgetPalletQuantity, "dispatchWidgetPalletQuantity"},
  {kDispatchPalletQuantity, "dispatchPalletQuantity"},
  {kFactoryIsFull, "factoryIsFull"},
  {kTruckIsFull, "truckIsFull"},
  {kInvalidRule, nullptr}
})

////////////////////////////////////////////////////////
// Rule Context
////////////////////////////////////////////////////////

struct RuleContext {
  Inventory* truck_inv = nullptr;
  Inventory* factory_inv = nullptr;
  int truck_id = -1;
  int factory_id = -1;
};
////////////////////////////////////////////////////////
// Rule
////////////////////////////////////////////////////////

class Rule {
public:
  virtual ~Rule() = default;

  void SetType(RuleType type) 
    { type_ = type; }
  RuleType GetType() const
    { return type_; }

  int GetProgress() {
    return 0;
  }

  void SetStep(int step)
    { step_ = step; }

  void SetAmount(int amount) 
    { amount_ = amount; }

  void SetWidgetId(int id) 
    { widget_id_ = id;}

  void DecreaseAmount() {
    amount_ = (amount_ - step_ > 0)  
      ? amount_ - step_ : 0;
  }

  void IncreaseAmount() 
    { amount_ += step_; }

  int GetWidgetId() 
    { return widget_id_; }

  int GetAmount() const
    { return amount_; }

  bool Evaluate(const RuleContext& context) {
    switch(type_) {
      case kReceiveWidgetPalletQuantity:
        if (!started_) {
          initial_ = context.factory_inv->GetWidgetPalletQuantity(widget_id_);
          started_ = true;
        }

        return context.factory_inv->GetWidgetPalletQuantity(widget_id_)
          >= (initial_ + amount_);
      break;
      case kDispatchWidgetPalletQuantity:
        if (!started_) {
          initial_ = context.truck_inv->GetWidgetPalletQuantity(widget_id_);
          started_ = true;
        }

        return context.truck_inv->GetWidgetPalletQuantity(widget_id_)
          >= (initial_ + amount_);
      break;
      default:
        return false;
    }
  }

private: 
  RuleType type_ = RuleType::kInvalidRule;
  int amount_ = 0;
  int widget_id_ = -1;
  int initial_ = -1;
  int step_ = 5;
  bool started_ = false;
};

////////////////////////////////////////////////////////
// Actions
////////////////////////////////////////////////////////

enum ActionType {
  kDispatchWidget = 0,
  kReceiveWidget,
  kInvalidAction
};

class Action {
public:
  virtual ~Action() = default;
  // virtual int GetProgress() = 0;

  void SetType(ActionType type) 
    { type_ = type; }
  ActionType GetType() const
    { return type_; }
  void SetWidgetId(int id)
    { widget_id_ = id; }
  int GetWidgetId() 
    { return widget_id_; }

  virtual bool Execute(const RuleContext& context) {
    switch(type_) {
      case kDispatchWidget:
        if (context.factory_inv->RemoveWidgetPallet(widget_id_)) {
          return context.truck_inv->AddWidgetPallet(widget_id_); //No Check
        }
        return false;
      break;
      case kReceiveWidget:
        if (context.truck_inv->RemoveWidgetPallet(widget_id_)) {
          return context.factory_inv->AddWidgetPallet(widget_id_); //No Check
        } return false;
        break;
      default:
        return false;
        break;
    }
  }

private:
  ActionType type_ = ActionType::kInvalidAction;
  int widget_id_;
};

class Target {
public:
  Target(Rule r, Action a): rule_(r), action_(a) {}
  ~Target() = default;
  
  bool RuleMet(const RuleContext& context) {
    return rule_.Evaluate(context);
  }

  bool PerformAction(RuleContext& context) {
    return action_.Execute(context);
  }

  Rule& GetRule() {
    return rule_;
  }
private:
  Rule rule_;
  Action action_;
};

class Plan {
public:
  Plan(RuleContext c) {
    context_ = c;
  }
  Plan() = default;

  void AddTarget(Target t) {
    targets_.push_back(t);
  }

  void AddTarget(Rule rule, Action action) {
    Target t(rule,action);
    targets_.push_back(t);
  }

  void RemoveTarget(Target t) {
    // targets_.erase(std::find(targets_.begin(), targets_.end(), t));
  }

  void SetContext(RuleContext context) 
    { context_ = context;}
  RuleContext GetContext() const
    {return context_;}

  void NextAction() {
    bool successful = false;
    for (Target t: targets_) {
      if (!t.RuleMet(context_)) {
        successful = t.PerformAction(context_);
      }
      if (successful) break;
    }
  }

  bool IsDone() {
    if (targets_.empty()) return false;

    for (Rule rule: rules_) {
      if (rule.Evaluate(context_)) {
        return true;
      }
    }

    for (Target t: targets_) {
      if (!t.RuleMet(context_)) {
        return false;
      }
    }

    return true;
  }

  std::vector<Target>& GetTargets() {
    return targets_;
  }

  int GetId() {
    return id_;
  }
  void SetId(int id) 
    {id_ = id;}

private:
  std::vector<Target> targets_ = {};
  std::vector<Rule> rules_ = {};
  RuleContext context_;
  int id_;
};

#endif


/*
Start plan
   |
   v
Has a global stop condition been met?
   | yes
   +----------> Leave
   |
   no
   |
   v
Execute current target
   |
   v
Is current target finished?
   | no
   +----------> continue
   |
   yes
   |
   v
Move to next target
   |
   v
Are there more targets?
   | no
   +----------> Leave
   |
   yes
   |
   +----------> next target
*/

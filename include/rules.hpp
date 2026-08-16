#ifndef RULES_HPP
#define RULES_HPP

#include <vector>
#include <set>

#include "widget.hpp"
#include "inventory.hpp"

////////////////////////////////////////////////////////
// Rule Context
////////////////////////////////////////////////////////

struct RuleContext {
    Inventory* truck_inv = nullptr;
    Inventory* factory_inv = nullptr;

    std::set<int>* whitelist = nullptr;
    std::set<int>* blacklist = nullptr;
};

////////////////////////////////////////////////////////
// Rule
////////////////////////////////////////////////////////

class Rule {
public:
    virtual ~Rule() = default;

    virtual bool Evaluate(const RuleContext& context) const = 0;
};

////////////////////////////////////////////////////////
// AND
////////////////////////////////////////////////////////

class AndRule : public Rule {
public:
    AndRule() = default;

    AndRule(std::vector<Rule*> rules)
        : rules_(rules) {
    }

    void Add(Rule* rule) {
        rules_.push_back(rule);
    }

    bool Evaluate(const RuleContext& context) const override {
        for (const auto& rule : rules_) {
            if (!rule->Evaluate(context)) {
                return false;
            }
        }

        return true;
    }

private:
    std::vector<Rule*> rules_;
};

////////////////////////////////////////////////////////
// OR
////////////////////////////////////////////////////////

class OrRule : public Rule {
public:
    OrRule() = default;

    OrRule(std::vector<Rule*> rules)
        : rules_(rules) {
    }

    void Add(std::vector<Rule*> rule) {
        rules_.insert(rules_.end(), rule.begin(), rule.end());
    }

    bool Evaluate(const RuleContext& context) const override {
        for (const auto& rule : rules_) {
            if (rule->Evaluate(context)) {
                return true;
            }
        }

        return false;
    }

private:
    std::vector<Rule*> rules_;
};

////////////////////////////////////////////////////////
// Leaf Rules
////////////////////////////////////////////////////////

// fill till x amount of W in factory inv
class FactoryWidgetAtLeast : public Rule {
public:
    FactoryWidgetAtLeast(int widget_id, int amount)
    : widget_id_(widget_id), amount_(amount) {}

    bool Evaluate(const RuleContext& context) const override {
        if(context.factory_inv == nullptr) {
            return false;
        }

        return context.factory_inv->WidgetQuantity(widget_id_)
               > amount_;
    }

private:
    int widget_id_;
    int amount_;
};

// fill till x amount of W in truck inv
class TruckWidgetAtLeast : public Rule {
public:
    TruckWidgetAtLeast(int widget_id, int amount)
        : widget_id_(widget_id), amount_(amount) {}

    bool Evaluate(const RuleContext& context) const override {
        if(context.truck_inv == nullptr) {
            return false;
        }

        return context.truck_inv->WidgetQuantity(widget_id_)
               > amount_;
    }
private:
    int widget_id_;
    int amount_;
};

class TruckIsFull : public Rule {
public:
    bool Evaluate(const RuleContext& context) const override {
        if(context.truck_inv == nullptr)
            return false;
        
        return context.truck_inv->IsFull();
    }
};

class FactoryIsFull : public Rule {
public:
    bool Evaluate(const RuleContext& context) const override {
        if(context.factory_inv == nullptr)
            return false;
        
        return context.factory_inv->IsFull();
    }
};

class UntilTruckUnloadsAmt : public Rule {

};

class UntilFactoryUnloadsAmt: public Rule {

};

////////////////////////////////////////////////////////
// Actions
////////////////////////////////////////////////////////

class Action {
public:
    virtual ~Action() = default;

    virtual bool Execute(const RuleContext& context) = 0;
};


class TruckLoadWidget : public Action {
public:
    TruckLoadWidget(int widget_id)
        : widget_id_(widget_id) {
    }

    bool Execute(const RuleContext& context) override {
        if(context.factory_inv->RemoveWidget(widget_id_)) {
            return context.truck_inv->AddWidget(widget_id_); //No Check
        }
        return false;
    }

private:
    int widget_id_;
};

class TruckUnLoadWidget : public Action {
public:
    TruckUnLoadWidget(int widget_id)
        : widget_id_(widget_id) {
    }

    bool Execute(const RuleContext& context) override {
        if(context.truck_inv->RemoveWidget(widget_id_)) {
            return context.factory_inv->AddWidget(widget_id_); //No Check
        }

        return false;
    }

private:
    int widget_id_;
};

class UnloadTruck : public Action {
public:
    bool Execute(const RuleContext& context) override {
        // Unload truck.
        return false;
    }
};


class UnloadFactory : public Action {
public:
    bool Execute(const RuleContext& context) override {
        // Unload factory.
        return false;
    }
};


class NothingAction : public Action {
public:
    bool Execute(const RuleContext&) override {
        // Nothing to do.
        return false;
    }
};

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

class Target {
public:
    Target(Rule* r, Action* a): rule_(r), action_(a) {}
    ~Target() {
        delete rule_;
        delete action_;
    }
    
    bool RuleMet(const RuleContext& context) {
        return rule_->Evaluate(context);
    }

    bool PerformAction(RuleContext& context) {
        return action_->Execute(context);
    }
private:
    Rule* rule_;
    Action* action_;
};

class Plan {
public:
    Plan(RuleContext c) {
        context_ = c;
    }
    void AddTarget(Target* t) {
        targets_.push_back(t);
    }

    void AddTarget(Rule* rule, Action* action) {
        Target* t = new Target(rule,action);
        targets_.push_back(t);
    }

    void NextAction() {
        bool successful = false;
        for(Target* t: targets_) {
            if(!t->RuleMet(context_)) {
                successful = t->PerformAction(context_);
            }
            if(successful) break;
        }
    }

    bool IsDone() {
        if(targets_.empty()) return false;

        for(Rule* rule: rules_) {
            if(rule->Evaluate(context_)) {
                return true;
            }
        }

        for(Target* t: targets_) {
            if(!t->RuleMet(context_)) {
                return false;
            }
        }

        return true;
    }

private:
    std::vector<Target*> targets_;
    std::vector<Rule*> rules_;
    RuleContext context_;
};

#endif

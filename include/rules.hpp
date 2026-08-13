#ifndef RULES_HPP
#define RULES_HPP

#include <vector>
#include <set>

#include "widget.hpp"
#include "inventory.hpp"

struct RuleContext {
    Inventory* truck_inv = nullptr;
    Inventory* factory_inv = nullptr;

    std::set<int>* whitelist = nullptr;
    std::set<int>* blacklist = nullptr;

    // {widget id, amount}
    std::pair<int, int> factory_amount_at = {-1,-1};
    std::pair<int, int> truck_amt_at = {-1,-1};

    // Add anything else your conditions need here.
    // Example:
    // Widget* widget = nullptr;
    // int item_id = 0;
};

class Rule {
public:
    virtual ~Rule() = default;

    virtual bool Evaluate(const RuleContext& context) const = 0;
};

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

// fill till x amount of W in factory inv
class UntilFactoryAmountAt : public Rule {
public:
    bool Evaluate(const RuleContext& context) const override {
        if(context.factory_amount_at.first == -1 || context.factory_amount_at.second == -1) {
            return false; //Should I return true or false here?
        }

        if(context.factory_inv->GetWidgetQuantity(context.factory_amount_at.first)
            >= context.factory_amount_at.second){
                return true;
            }

        return false;
    }
};

// fill till x amount of W in truck inv
class TruckWidgetAmtAt : public Rule {
public:
    bool Evaluate(const RuleContext& context) const override {
        if(context.truck_amt_at.first == -1 || context.truck_amt_at.second == -1) {
            return false; //Should I return true or false here?
        }

        if(context.factory_inv->GetWidgetQuantity(context.truck_amt_at.first)
            >= context.truck_amt_at.second){
                return true;
            }

        return false;
    }
};

class TruckIsFull : public Rule {
    bool Evaluate(const RuleContext& context) const override {
        return context.truck_inv->IsFull();
    }
};

class FactoryIsFull : public Rule {
    bool Evaluate(const RuleContext& context) const override {
        return context.factory_inv->IsFull();
    }
};

class UntilTruckUnloadsAmt : public Rule {

};

class UntilFactoryUnloadsAmt: public Rule {

};


// Unload X amount
class RulesEngine {
    public:
        RulesEngine() {

        }
    private:

};

#endif
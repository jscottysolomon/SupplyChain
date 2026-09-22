#ifndef UI_HANDLER_HPP
#define UI_HANDLER_HPP

#include <vector>

#include "factory.hpp"
#include "scheduler.hpp"
#include "truck.hpp"
#include "traffic.hpp"

bool Style(Rule* rule, Truck* truck, Factory* factory);
std::string CreateUniqueId(std::string name, Truck* t, Factory* f, int id);
std::string CreateUniqueId(std::string name, int id);
std::string CreateUniqueId(std::string name, int id);
void StyleEnd(bool style);

class GameUi {
  public:
  GameUi(TrafficCommand& traffic, LogisticsController& scheduler)
    : commander_(traffic), logisticsController_(scheduler) {
        organizer_ = ReceipeOrganizer::GetInstance();
    }
    void RenderUi();
  private:
    TrafficCommand& commander_;
    LogisticsController& logisticsController_;

    // Truck& truck_;
    std::size_t truck_index_ = 0;
    int truck_id_ = -1;

    // Factory& factory_;
    std::size_t factory_index_ = 0;
    int factory_id_ = -1;
    int last_clicked_factory_id_;
    bool capture_next_factory;
    
    ReceipeOrganizer* organizer_;

    void TruckWidget();
    void PlanRuleMenu(Factory* factory, Plan p);
    void TruckScheduleTab();
    void TargetTableRow(Target& target,Factory* factory, std::vector<Target>& removals);
    void DispatchWidgetPalletQuantity(Rule& rule, Target& target, std::vector<Target>& removals);
    void FactoryWidget();
    void BaseLayer();

    void ReceiveWidgetPalletQuantity(Rule& rule, Target& target, std::vector<Target>& removals);
};

#endif
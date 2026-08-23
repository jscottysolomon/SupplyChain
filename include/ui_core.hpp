#ifndef UI_HANDLER_HPP
#define UI_HANDLER_HPP

#include <vector>

#include "truck.hpp"
#include "factory.hpp"
#include "traffic_mediator.hpp"

bool Style(Rule* rule, Truck* truck, Factory* factory);
std::string CreateUniqueId(std::string name, Truck* t, Factory* f, int id);
std::string CreateUniqueId(std::string name, int id);
std::string CreateUniqueId(std::string name, int id);
void StyleEnd(bool style);

class UiHandler {
  public:
  UiHandler(TrafficMediator& traffic)
    : controller_(traffic), truck_(nullptr), truck_id_(-1),
      factory_(nullptr), factory_id_(-1) {
        trucks_ = traffic.GetTrucks();
        factories_ = traffic.GetFactories();
        truck_index_ = 0;
        factory_index_ = 0;
        organizer_ = ReceipeOrganizer::GetInstance();
    }
    void RenderUi();
  private:
    TrafficMediator& controller_;

    std::vector<Truck*> trucks_;
    Truck* truck_;
    std::size_t truck_index_;
    int truck_id_;

    std::vector<Factory*> factories_;
    Factory* factory_;
    std::size_t factory_index_;
    int factory_id_;
    
    ReceipeOrganizer* organizer_;

    void TruckWidget();
    void TruckScheduleFactorySection(Factory* factory, Plan* p);
    void TruckScheduleTab();
    void TargetTableRow(Target* target,Factory* factory,
        std::vector<Target*>& removals);
    void QuantityTargetTableRow(std::string str, DispatchQuantity* rule, Target* target, 
        Factory* factory, std::vector<Target*>& removals);
    void UiHandler::QuantityTargetTableRow(std::string str, ReceiveQuantity* rule, Target* t, 
        Factory* f, std::vector<Target*>& removals);
    void FactoryWidget();
    void BaseLayer();
};

#endif
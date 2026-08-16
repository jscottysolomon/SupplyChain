#ifndef UI_HANDLER_HPP
#define UI_HANDLER_HPP

#include <vector>

#include "truck.hpp"
#include "factory.hpp"
#include "traffic_mediator.hpp"

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
    void TruckWidget();
    void FactoryWidget();
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
};

#endif
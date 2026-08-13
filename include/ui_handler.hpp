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
                truck_index_ = 0;
        }
        void RenderUi();
        void TruckWidget();
    private:
        TrafficMediator& controller_;
        Truck* truck_;
        int truck_id_;
        Factory* factory_;
        int factory_id_;
        int truck_index_;
        std::vector<Truck*> trucks_;
};

#endif
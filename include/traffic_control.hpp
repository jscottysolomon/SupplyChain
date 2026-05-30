#ifndef TRAFFIC_CONTROLLER_HPP
#define TRAFFIC_CONTROLLER_HPP

#include <vector>
#include <queue>

class Factory;
class Intersection;
class Truck;
struct Dock;

struct Vertex{
    Intersection* inter;
    float dist;
    Vertex* prev;
};

class TrafficControl {
    public:
        TrafficControl(std::vector<Intersection*>& intersections)
        : intersections_(intersections)
        {

        }
        std::queue<Intersection*> RequestRoute(Intersection* src, Intersection* dest);
        bool RequestIntersection(Intersection* inter, Truck* truck);
        Dock* RequestDock(Factory* factory, Truck* truck);
    private:
        std::vector<Intersection*>& intersections_;
        std::vector<Vertex*> Dijkstra(Intersection* src);
};

#endif
#ifndef TRAFFIC_CONTROLLER_HPP
#define TRAFFIC_CONTROLLER_HPP

#include <vector>

class Intersection;

struct Vertex{
    Intersection* inter;
    float dist;
    Vertex* prev;
};

class TrafficControl {

    private:
        std::vector<Intersection*>& intersections_;
        std::vector<Vertex*> Dijkstra(Intersection* src);
    public:
        TrafficControl(std::vector<Intersection*>& intersections)
        : intersections_(intersections)
        {
            
        }
        std::vector<Intersection*> RequestRoute(Intersection* src, Intersection dest);

};

#endif
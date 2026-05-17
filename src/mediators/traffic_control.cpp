#include "traffic_control.hpp"

#include <float.h>
#include <iostream>
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include <vector>

#include "intersection.hpp"

Vertex* GetVertex(std::vector<Vertex*> vertices, Intersection* intersection);

std::vector<Intersection*> TrafficControl::RequestRoute(Intersection* src, Intersection* dest) {
    std::vector<Vertex*> vertices = Dijkstra(src);
    std::vector<Intersection*> route;

    Vertex* v = GetVertex(vertices,dest);

    while(v != nullptr) {
        route.push_back(v->inter);
        v = v->prev;
    }

    for(Vertex* v: vertices) {
        delete v;
    }

    std::reverse(route.begin(),route.end());

    return route;
}

std::vector<Vertex*> TrafficControl::Dijkstra(Intersection* src)
{

    std::vector<Vertex*> vertices;
    std::vector<Vertex*> unvisited;

    int sizee = intersections_.size();

    for(Intersection* inter: intersections_) {
        struct Vertex* v = new Vertex;
        v->inter = inter;
        v->dist = FLT_MAX;
        v->prev = nullptr;
        
        if(inter == src) {
            v->dist = 0;
        }

        vertices.push_back(v);
        unvisited.push_back(v);
    }

    while(!unvisited.empty()) {
        //finding lowest distance of unvisited Vertex
        int rmv = 0;
        Vertex* u = unvisited[0];
        for(std::size_t ii = 0; ii < unvisited.size(); ii++) {
            if(unvisited.at(ii)->dist < u->dist) {
                u = unvisited.at(ii);
                rmv = ii;
            }
        }

        //swap and pop
        unvisited[rmv] = unvisited.back();
        unvisited.pop_back();

        //Updating u's neighbors
        for(Intersection* inter: u->inter->GetIntersections()) {
            Vertex* neighbor = GetVertex(vertices,inter);

            if(neighbor != nullptr) {
                float alt = u->dist + Vector2Distance(u->inter->GetPosition(), 
                    neighbor->inter->GetPosition());

                if(alt < neighbor->dist) {
                    neighbor->dist = alt;
                    neighbor->prev = u;
                }
            }
        }
    }

    return vertices;
}

Vertex* GetVertex(std::vector<Vertex*> vertices, Intersection* intersection) {
    for(Vertex* v: vertices) {
        if(v->inter == intersection) {
            return v;
        }
    }

    return nullptr;
}
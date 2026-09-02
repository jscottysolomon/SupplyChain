#include "traffic.hpp"

#include <cassert>
#include <float.h>
#include <iostream>
#include <list>
#include <memory>
#include <vector>
#include <queue>


#include <graaflib/algorithm/shortest_path/dijkstra_shortest_path.h>
#include <graaflib/algorithm/shortest_path/bfs_shortest_path.h>
#include <raylib.h>
#include <raymath.h>

#include "factory.hpp"
#include "intersection.hpp"
#include "truck.hpp"

Vertex* GetVertex(std::vector<Vertex*> vertices, Intersection* intersection);

std::queue<Intersection*> TrafficMediator::RequestRoute(Intersection* src, Intersection* dest) {
  std::vector<Vertex*> vertices = Dijkstra(src);
  std::vector<Intersection*> route;

  Vertex* v = GetVertex(vertices,dest);

  while (v != nullptr) {
    route.push_back(v->inter);
    v = v->prev;
  }

  for (Vertex* v: vertices) {
    delete v;
  }

  std::queue<Intersection*> ret;

  for (auto it = route.rbegin(); it != route.rend(); ++it) {
    ret.push(*it);
  }

  return ret;
}

std::list<Junction*> TrafficMediator::RequestRoute(Junction* src, Junction* dest) {
  std::list<Junction*> path;

  if(src == nullptr || dest == nullptr) {
    TraceLog(LOG_WARNING, "Passed null junction for path request");
    return path;
  }

  const auto maybe_path = graaf::algorithm::bfs_shortest_path(graph_, 
      src->GetGraphId(), dest->GetGraphId());

  assert(maybe_path.has_value());
  auto shortest_path{maybe_path.value()};

  
  for (graaf::vertex_id_t id: shortest_path.vertices) {
    Junction* node = graph_.get_vertex(id);
    if(node != nullptr) {
      path.push_back(node);
    } else {
      TraceLog(LOG_WARNING, "Null pointer in path");
    }
  }

  return path;
}

std::vector<Vertex*> TrafficMediator::Dijkstra(Intersection* src)
{

  std::vector<Vertex*> vertices;
  std::vector<Vertex*> unvisited;

  int sizee = intersections_.size();

  for (Intersection* inter: intersections_) {
    struct Vertex* v = new Vertex;
    v->inter = inter;
    v->dist = FLT_MAX;
    v->prev = nullptr;
    
    if (inter == src) {
      v->dist = 0;
    }

    vertices.push_back(v);
    unvisited.push_back(v);
  }

  while (!unvisited.empty()) {
    //finding lowest distance of unvisited Vertex
    int rmv = 0;
    Vertex* u = unvisited[0];
    for (std::size_t ii = 0; ii < unvisited.size(); ii++) {
      if (unvisited.at(ii)->dist < u->dist) {
        u = unvisited.at(ii);
        rmv = ii;
      }
    }

    //swap and pop
    unvisited[rmv] = unvisited.back();
    unvisited.pop_back();

    //Updating u's neighbors
    for (Intersection* inter: u->inter->GetIntersections()) {
      Vertex* neighbor = GetVertex(vertices,inter);

      if (neighbor != nullptr) {
        float alt = u->dist + Vector2Distance(u->inter->GetPosition(), 
          neighbor->inter->GetPosition());

        if (alt < neighbor->dist) {
          neighbor->dist = alt;
          neighbor->prev = u;
        }
      }
    }
  }

  return vertices;
}

Vertex* GetVertex(std::vector<Vertex*> vertices, Intersection* intersection) {
  for (Vertex* v: vertices) {
    if (v->inter == intersection) {
      return v;
    }
  }

  return nullptr;
}

Dock* TrafficMediator::RequestDock(Junction* junction, Truck* truck) {
  if(junction->GetType() == JunctionType::Factory) {
    if (auto* factory = dynamic_cast<Factory*>(junction)) {
      return factory->DockRequest(truck);
    }
  }
  return nullptr;
}

Dock* TrafficMediator::RequestDock(Factory* factory, Truck* truck) {
  return factory->DockRequest(truck);
}
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
#include "truck.hpp"

Vertex* GetVertex(std::vector<Vertex*> vertices, Intersection* intersection);

std::list<int> TrafficService::RequestRoute(int id, Junction* dest) {
  std::list<int> path;
  Junction* src = commander_.GetJunction(id);

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
      path.push_back(node->GetId());
    } else {
      TraceLog(LOG_WARNING, "Null pointer in path");
    }
  }

  return path;
}

Vertex* GetVertex(std::vector<Vertex*> vertices, Intersection* intersection) {
  for (Vertex* v: vertices) {
    if (v->inter == intersection) {
      return v;
    }
  }

  return nullptr;
}

Dock* TrafficService::RequestDock(Junction* junction, Truck* truck) {
  if(junction->GetType() == JunctionType::Factory) {
    if (auto* factory = dynamic_cast<Factory*>(junction)) {
      return factory->DockRequest(truck);
    }
  }
  return nullptr;
}

Dock* TrafficService::RequestDock(Factory* factory, Truck* truck) {
  return factory->DockRequest(truck);
}

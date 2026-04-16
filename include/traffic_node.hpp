#ifndef TRAFFIC_NODE_HPP

#define TRAFFIC_NODE_HPP

#include <vector>
#include <algorithm>

class TrafficNode  {
	private:
		std::vector<TrafficNode*> connections_;
	public:
        TrafficNode() {}
		std::vector<TrafficNode*> GetNodes() {
            return connections_;
        }
        
        void AddNode(TrafficNode* node) {
            connections_.push_back(node);
        }

        void RemoveNode(TrafficNode* node) {
            connections_.erase(std::remove(connections_.begin(), connections_.end(), node), connections_.end());
        }

};

#endif
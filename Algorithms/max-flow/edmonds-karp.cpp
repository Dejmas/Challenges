#include <vector>
#include <deque>
#include <iostream>
#include <set>

#include "flow-network.h"

// parallel edges, counter edges
class EdmondsKarp : public FlowNetwork {
    public: 
        EdmondsKarp(size_t noNodes);
        void addEdge(int a, int b, long cap) override;
        long maxFlow(int source, int target) override;

        inline static long INF = 1e18; 

        struct Edge {
            int from, to;
            long capacity, flow = 0, cost;

            long residue() const {
                return capacity - flow;
            }
        };

    private:
        long augmentingPath(int source, int target, std::vector<int> & parent) const;

        std::vector<std::vector<int>> m_adjacent;
        std::vector<Edge> m_edges;
};

EdmondsKarp::EdmondsKarp(size_t noNodes) 
: m_adjacent(noNodes)
{
}

void EdmondsKarp::addEdge(int a, int b, long cap) {
    auto m = m_edges.size();
    m_edges.push_back(Edge{a, b, cap, 0, 0});
    m_adjacent[a].push_back(m ++);
    m_edges.push_back(Edge{b, a, 0, 0, 0});
    m_adjacent[b].push_back(m);
}

long EdmondsKarp::maxFlow(int source, int target) {
    long flow = 0;
    std::vector<int>parent(m_adjacent.size());
    long path_flow;
    while ((path_flow = augmentingPath(source, target, parent)) > 0 ) {
        flow += path_flow;
        // std::cerr << "augmenting path " << path_flow << ": ";
        for (int current = target; current != source; ) {
            int id = parent[current];
            m_edges[id].flow += path_flow;
            m_edges[id ^ 1].flow -= path_flow;
            // std::cerr << current << " -> " << m_edges[id].from << " ";
            current = m_edges[id].from;
        }
        // std::cerr << "\n";
    }

    return flow;
}

long EdmondsKarp::augmentingPath(int source, int target, std::vector<int> & parent) const{
    parent.assign(parent.size(), -1);
    parent[source] = -2;
    std::deque<std::pair<int, long>> q({{source, INF}});

    while (!q.empty()) {
        auto [current, flow] = q.front();
        q.pop_front();

        for (int idx : m_adjacent[current]) {
            auto e = m_edges[idx];
            if (parent[e.to] == -1 && e.residue() > 0) {
                long local_flow = std::min(flow, e.residue());
                parent[e.to] = idx;
                if (e.to == target) return local_flow;
                q.push_back({e.to, local_flow});
            }
        }
    }

    return 0;
}

#include "flow-network-test.h"

int main () {
    testMaxFlow<EdmondsKarp>();
}

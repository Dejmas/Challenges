#include <vector>
#include <deque>
#include <iostream>
#include <set>

#include "flow-network.h"

class EdmondsKarpSimple : public FlowNetwork {
    public: 
        EdmondsKarpSimple(size_t noNodes);
        void addEdge(int a, int b, long cap) override;
        long maxFlow(int source, int target) override;

    private:
        long augmentingPath(int source, int target, std::vector<int> & parent) const;

        std::vector<std::vector<int>> m_adjacent;
        std::vector<std::vector<long>> m_residues;
        std::set<std::pair<int, int>> m_existingEdges;
};

EdmondsKarpSimple::EdmondsKarpSimple(size_t noNodes) 
: m_adjacent(noNodes)
, m_residues(noNodes, std::vector<long>(noNodes, 0)) {
}

void EdmondsKarpSimple::addEdge(int a, int b, long cap) {
    if (!m_existingEdges.contains({a,b})) {
        m_adjacent[a].push_back(b);
        m_adjacent[b].push_back(a);
        m_residues[a][b] = cap;
        m_existingEdges.insert({a,b});
    } else {
        m_residues[a][b] += cap;
    }
}

long EdmondsKarpSimple::maxFlow(int source, int target) {
    long flow = 0;
    std::vector<int>parent(m_adjacent.size());
    long path_flow;
    while ((path_flow = augmentingPath(source, target, parent)) > 0 ) {
        flow += path_flow;
        //std::cerr << "augmenting path " << path_flow << ": ";
        for (int current = target; current != source; ) {
            int prev = parent[current];
            m_residues[prev][current] -= path_flow;
            m_residues[current][prev] += path_flow;
            //std::cerr << current << " -> " << prev << " ";
            current = prev;
        }
        //std::cerr << "\n";
    }

    return flow;
}

long EdmondsKarpSimple::augmentingPath(int source, int target, std::vector<int> & parent) const{
    parent.assign(parent.size(), -1);
    parent[source] = -2;
    std::deque<std::pair<int, long>> q({{source, INF}});

    while (!q.empty()) {
        auto [current, flow] = q.front();
        q.pop_front();

        for (int next : m_adjacent[current]) {
            if (parent[next] == -1 && m_residues[current][next] > 0) {
                long local_flow = std::min(flow, m_residues[current][next]);
                parent[next] = current;
                if (next == target) return local_flow;
                q.push_back({next, local_flow});
            }
        }
    }

    return 0;
}

#include "flow-network-test.h"

int main () {
    testMaxFlow<EdmondsKarpSimple>();
}

#include <vector>
#include <deque>
#include <iostream>
#include <set>

#include "flow-network.h"


class Dinitz : public FlowNetwork {
    public: 
        Dinitz(size_t noNodes);
        void addEdge(int a, int b, long cap) override;
        long maxFlow(int source, int target) override;

        struct Edge {
            int from, to;
            long capacity, cost, flow = 0;

            long residue() const {
                return capacity - flow;
            }
        };

    private:
        long dfs_and_cleanup(int source, int target, long flow = INF);
        bool bfs(int source, int target);

        std::vector<std::vector<int>> m_adjacent;
        std::vector<std::vector<int>> m_clean_adjacent;
        std::vector<Edge> m_edges;
};

Dinitz::Dinitz(size_t noNodes) 
: m_adjacent(noNodes)
{
}

void Dinitz::addEdge(int a, int b, long cap) {
    auto m = m_edges.size();
    m_edges.push_back(Edge{a, b, cap, 0, 0});
    m_adjacent[a].push_back(m);
    m_edges.push_back(Edge{b, a, 0, 0, 0});
    m_adjacent[b].push_back(m + 1);
}

bool Dinitz::bfs(int start, int target) {
    size_t N = m_adjacent.size();
    std::vector<long> distance(N, INF);
    m_clean_adjacent.assign(N, {});
    std::deque<int> queue = {start};
    distance[start] = 0;

    while (!queue.empty()) {
        int current = queue.front();
        queue.pop_front();

        if (distance[current] + 1 > distance[target]) break;

        for (int idx : m_adjacent[current]) {
            Edge edge = m_edges[idx];
            if (edge.residue() > 0 && distance[edge.from] < distance[edge.to]) {
                m_clean_adjacent[edge.from].push_back(idx);
                if (distance[edge.to] == INF) { // not visited
                    distance[edge.to] = distance[edge.from] + 1;
                    queue.push_back(edge.to);
                }
            }
        }
    }

    return distance[target] != INF;
}

long Dinitz::dfs_and_cleanup(int node, int target, long flow) {
    if (!flow || node == target) {
        return flow;
    }
    for (int i = m_clean_adjacent[node].size() -1; i >= 0; -- i) {
        int idx = m_clean_adjacent[node][i];
        Edge edge = m_edges[idx];
        if (edge.residue() > 0) {
            long path_flow = dfs_and_cleanup(edge.to, target, std::min(flow, edge.residue()));
            if (path_flow > 0) {
                m_edges[idx].flow += path_flow;
                m_edges[idx ^ 1].flow -= path_flow;
                return path_flow;
            }
        }
        m_clean_adjacent[node].pop_back();
    }
    return 0;
}

long Dinitz::maxFlow(int start, int target) {
    long flow = 0;
    while (bfs(start, target)) {
        long path_flow;
        while ((path_flow = dfs_and_cleanup(start, target, INF))) {
            flow += path_flow;
        }
    }

    return flow;
}

#include "flow-network-test.h"

int main () {
    testMaxFlow<Dinitz>();
}

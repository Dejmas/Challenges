#include <vector>
#include <limits>
#include "flow-network.h"



class PushRelabel : public FlowNetwork {
public:

    PushRelabel(int N);

    void addEdge(int a, int b, long cap) override;
    long maxFlow(int source, int target) override;

    struct Edge {
        int from, to;
        long capacity, cost, flow = 0;

        long residue() const { return capacity - flow; }
    };

private:

    bool push(int from);
    void relabel(int node);
    std::vector<int> getHighestNodes(int from, int to) const;

    std::vector<std::vector<int>> m_adjacent;
    std::vector<Edge> m_edges;
    std::vector<long> m_excess;
    std::vector<int> m_height;

};

PushRelabel::PushRelabel(int N) : m_adjacent(N) {}

void PushRelabel::addEdge(int a, int b, long cap) {
    auto M = m_edges.size();
    m_edges.push_back(Edge{a, b, cap});
    m_adjacent[a].push_back(M ++);
    m_edges.push_back(Edge{b, a, 0});
    m_adjacent[b].push_back(M);
}

long PushRelabel::maxFlow(int source, int target) {
    auto N = m_adjacent.size();
    m_height.assign(N, 0);
    m_excess.assign(N, 0);
    m_height[source] = N;
    m_excess[source] = INF;

    push(source);

    std::vector<int> highest_nodes;
    while ((highest_nodes = getHighestNodes(source, target)).empty() == false) {
        for (int current : highest_nodes) {
            if (!push(current)) {
                relabel(current);
            }
        }
    }

    return m_excess[target];
}

bool PushRelabel::push(int from) {
    bool pushed = false;
    for (int idx : m_adjacent[from]) {
        Edge & edge = m_edges[idx];
        if (m_height[from] > m_height[edge.to] && edge.residue() > 0) {
            long push_by = std::min(m_excess[from], edge.residue());
            m_excess[from] -= push_by;
            m_excess[edge.to] += push_by;
            m_edges[idx].flow += push_by;
            m_edges[idx ^ 1].flow -= push_by;
            pushed = true;
        }
    }

    return pushed;
}

void PushRelabel::relabel(int node) {
    int minimum = std::numeric_limits<int>::max();
    for (int idx : m_adjacent[node]) {
        if (m_edges[idx].residue() > 0)
            minimum = std::min(minimum, m_height[m_edges[idx].to]);
    }
    m_height[node] = minimum + 1;
}

std::vector<int> PushRelabel::getHighestNodes(int from, int to) const {
    std::vector<int> result;
    int maximum = 0;
    for (int i = 0; i < (int)m_adjacent.size(); i ++) {
        if (i == from || i == to || m_excess[i] == 0) continue;
        if (m_height[i] == maximum) {
            result.push_back(i);
        }
        if (m_height[i] > maximum) {
            result = { i };
            maximum = m_height[i];
        }
    }
    return result;
}

#include "flow-network-test.h"

int main () {
    testMaxFlow<PushRelabel>();
}
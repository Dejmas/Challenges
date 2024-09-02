#include <vector>
#include <deque>
#include <iostream>
#include <set>
#include <cassert>

// This implementation is modified Edmonds-Karp for computin max flow.
// Edges of negative costs are allowed.
// Algorithm to find the shortest path (in terms of costs) is called Shortest Path Faster Algorithm (SPFA), because of negative costs. 
// No parallel edges, each pair of node can only have edge in one direction or none.
class NetworkCostFlow {
    public: 
        NetworkCostFlow(size_t noNodes);
        void addEdge(int a, int b, long cap, long cost);
        long minCostFlow(int source, int target, long flowLimit);

        inline static long INF = 1e18; 

    private:
        void augmentingPath(int source, int target, std::vector<int> & parents, std::vector<long> & costs) const;

        std::vector<std::vector<int>> m_adjacent;
        std::vector<std::vector<long>> m_residues;
        std::vector<std::vector<long>> m_cost;
        std::set<std::pair<int, int>> m_existingEdges;
};

NetworkCostFlow::NetworkCostFlow(size_t noNodes) 
: m_adjacent(noNodes)
, m_residues(noNodes, std::vector<long>(noNodes, 0))
, m_cost(noNodes, std::vector<long>(noNodes, 0)) {
}

void NetworkCostFlow::addEdge(int a, int b, long cap, long cost) {
    if (!m_existingEdges.contains({a,b}) && !m_existingEdges.contains({b,a})) {
        m_adjacent[a].push_back(b);
        m_adjacent[b].push_back(a);
        m_residues[a][b] = cap;
        m_cost[a][b] = cost;
        m_cost[b][a] = -cost;
        m_existingEdges.insert({a,b});
    } else {
        throw std::invalid_argument("either parallel edge or counter edge to some other.");
    }
}

long NetworkCostFlow::minCostFlow(int source, int target, long flowLimit) {
    long flow = 0;
    long cost = 0;

    std::vector<int> parents;
    std::vector<long> costs;
    
    while (flow < flowLimit) {
        augmentingPath(source, target, parents, costs);
        if (costs[target] == INF) break;

        // find max flow
        long path_flow = flowLimit - flow;
        for (int current = target; current != source; ) {
            int prev = parents[current];
            path_flow = std::min(path_flow, m_residues[prev][current]);
            current = prev;
        }
 
        // apply flow
        flow += path_flow;
        cost += path_flow * costs[target];
        // std::cout << "augmenting path " << path_flow << ": ";
        for (int current = target; current != source; ) {
            int prev = parents[current];
            m_residues[prev][current] -= path_flow;
            m_residues[current][prev] += path_flow;
            // std::cout << current << " -> " << prev << " ";
            current = prev;
        }
        // std::cout << "\n";
    }

    if (flow < flowLimit) return -1;
    std::cout << "flow " << flow << " cost " << cost << "\n";
    return cost;
}

void NetworkCostFlow::augmentingPath(int source, int target, std::vector<int> & parents, std::vector<long> & costs) const{
    size_t N = m_adjacent.size();
    costs.assign(N, INF);
    parents.assign(N, -1);
    std::vector<bool> inQ(N, false);
    std::deque<int> q = {source};
    costs[source] = 0;
    while (!q.empty()) {
        auto current = q.front();
        q.pop_front();
        inQ[current] = false;

        for (int next : m_adjacent[current]) {
            if (m_residues[current][next] > 0 && costs[next] > costs[current] + m_cost[current][next]) {
                costs[next] = costs[current] + m_cost[current][next];
                parents[next] = current;
                if (!inQ[next]) {
                    inQ[next] = true;
                    q.push_back(next);
                }
            }
        }
    }
}


void testMaxFlow () {
    NetworkCostFlow network(10);
    network.addEdge(0, 1, 7, 1);
    network.addEdge(1, 2, 5, 1);
    network.addEdge(2, 5, 8, 1);
    network.addEdge(1, 3, 3, 1);
    network.addEdge(0, 4, 4, 1);
    network.addEdge(4, 1, 3, 1);
    network.addEdge(4, 3, 2, 1);
    network.addEdge(3, 2, 3, 1);
    network.addEdge(3, 5, 5, 1);

    auto flow = network.minCostFlow(0, 5, 10);
    std::cout << "Max flow is " << flow << "\n";
}

// https://www.spoj.com/problems/GREED/
// But there is a problem, exchanges are bidirectional, which this implementation of MinCutMaxFlow does not support.
void GREED_Greedy_island () {
    NetworkCostFlow network(6);
    network.addEdge(0, 1, 1, 0);
    network.addEdge(0, 2, 3, 0);
    network.addEdge(0, 3, 0, 0);
    network.addEdge(0, 4, 0, 0);

    network.addEdge(2, 3, NetworkCostFlow::INF, 1);
    network.addEdge(3, 4, NetworkCostFlow::INF, 1);

    network.addEdge(1, 5, 1, 0);
    network.addEdge(2, 5, 1, 0);
    network.addEdge(3, 5, 1, 0);
    network.addEdge(4, 5, 1, 0);

    auto flow = network.minCostFlow(0, 5, 4);
    std::cout << flow << "\n";
    assert(flow == 3);
}

void testChat() {
    NetworkCostFlow g(6);
    g.addEdge(0, 1, 10, 2);
    g.addEdge(0, 2, 5, 6);
    g.addEdge(1, 2, 15, 1);
    g.addEdge(1, 3, 10, 4);
    g.addEdge(2, 4, 10, 2);
    g.addEdge(3, 4, 10, 3);
    g.addEdge(3, 5, 10, 1);
    g.addEdge(4, 5, 10, 2);

    auto cost = g.minCostFlow(0, 5, 15);
    std::cout << cost << "\n";
}

int main () {
    testChat();
    // testMaxFlow();
    //GREED_Greedy_island();
}

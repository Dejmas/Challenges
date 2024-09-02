#include <vector>
#include <deque>
#include <iostream>
#include <set>

// Proper implementation
class CostFlowNetwork {
    public: 
        CostFlowNetwork(size_t noNodes);
        void addEdge(int a, int b, long cap, long cost);
        long minCostFlow(int source, int target, long flowLimit);

        struct Edge {
            int from, to;
            long capacity, flow = 0, cost;
            Edge (int a, int b, long capacity, long cost)
            : from(a), to(b), capacity(capacity), cost(cost) {
            }

            long residue() const {
                return capacity - flow;
            }
        };

        inline static long INF = 1e18;

    private:
        void augmentingPath(int source, int target, std::vector<int> & parents, std::vector<long> & costs) const;

        std::vector<std::vector<int>> m_adjacent;
        std::vector<Edge> m_edges;
};

CostFlowNetwork::CostFlowNetwork(size_t noNodes) 
: m_adjacent(noNodes){
}

void CostFlowNetwork::addEdge(int a, int b, long cap, long cost) {
    auto m = m_edges.size();
    m_edges.emplace_back(Edge{a, b, cap, cost});
    m_adjacent[a].push_back(m ++);
    m_edges.emplace_back(Edge{b, a, 0, -cost});
    m_adjacent[b].push_back(m);
}

long CostFlowNetwork::minCostFlow(int source, int target, long flowLimit) {
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
            Edge edge = m_edges[parents[current]];
            path_flow = std::min(path_flow, edge.residue());
            current = edge.from;
        }

        // apply flow
        flow += path_flow;
        cost += path_flow * costs[target];
        // std::cerr << "augmenting path " << path_flow << ": ";
        for (int current = target; current != source; ) {
            int idx = parents[current];
            m_edges[idx].flow += path_flow;
            m_edges[idx ^ 1].flow -= path_flow;
            // std::cerr << current << " -> " << m_edges[idx].from << " ";
            current = m_edges[idx].from;
        }
        // std::cerr << "\n";
    }

    // std::cerr << "flow " << flow << " cost " << cost << "\n";
    if (flow < flowLimit) return -1;
    return cost;
}

void CostFlowNetwork::augmentingPath(int source, int target, std::vector<int> & parents, std::vector<long> & costs) const{
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

        for (int idx : m_adjacent[current]) {
            auto edge = m_edges[idx];
            if (edge.residue() > 0 && costs[edge.to] > costs[current] + edge.cost) {
                costs[edge.to] = costs[current] + edge.cost;
                parents[edge.to] = idx;
                if (!inQ[edge.to]) {
                    inQ[edge.to] = true;
                    q.push_back(edge.to);
                }
            }
        }
    }
}

// https://www.spoj.com/problems/GREED/
bool GREED_Greedy_island () {
    int N;
    if (!(std::cin >> N)) {
        return false;
    }
    std::vector<int> cards(N+1, 0);
    for (int i = 0; i < N; i ++) {
        unsigned card;
        if (!(std::cin >> card)) {
            return false;
        }
        cards[card] ++;
    }

    const int source = 0;
    const int target = N+1;

    CostFlowNetwork network(N+2);

    for (int i = 1; i <= N; i ++) {
        network.addEdge(source, i, cards[i], 0);
        network.addEdge(i, target, 1, 0);
    }

    int exchanges;
    if (!(std::cin >> exchanges)) {
        return false;
    }

    while (exchanges -- > 0) {
        int from, to;
        if (!(std::cin >> from >> to)) {
            return false;
        }
        network.addEdge(from, to, CostFlowNetwork::INF, 1);
        // unfortunately
        network.addEdge(to, from, CostFlowNetwork::INF, 1);
    }

    auto flow = network.minCostFlow(source, target, N);
    std::cout << flow << "\n";
    return true;
}

int main () {
    int tests;
    if (!(std::cin >> tests)) {
        std::cerr << "Invalid input.\n";
        return 1;
    }
    while (tests -- > 0) {
        bool good = GREED_Greedy_island();
        if (!good) {
            std::cerr << "Invalid input.\n";
            return 2;
        }
    }
}

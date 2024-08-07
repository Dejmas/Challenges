#include <vector>
#include <deque>
#include <iostream>
#include <set>

class NetworkCostFlow {
    public: 
        NetworkCostFlow(size_t noNodes);
        void addEdge(int a, int b, long cap, long cost);
        long minCostFlow(int source, int target, long flowLimit);

        static long INF; 

    private:
        void augmentingPath(int source, int target, std::vector<int> & parents, std::vector<long> & costs) const;

        std::vector<std::vector<int>> m_adjacent;
        std::vector<std::vector<long>> m_residues;
        std::vector<std::vector<long>> m_cost;
        std::set<std::pair<int, int>> m_existingEdges;
};

long NetworkCostFlow::INF = 1e18;

NetworkCostFlow::NetworkCostFlow(size_t noNodes) 
: m_adjacent(noNodes)
, m_residues(noNodes, std::vector<long>(noNodes, 0))
, m_cost(noNodes, std::vector<long>(noNodes, 0)) {
}

void NetworkCostFlow::addEdge(int a, int b, long cap, long cost) {
    if (m_existingEdges.count({a,b}) == 0 && m_existingEdges.count({b,a}) == 0) {
        m_adjacent[a].push_back(b);
        m_adjacent[b].push_back(a);
        m_residues[a][b] = cap;
        m_cost[a][b] = cost;
        m_cost[b][a] += -cost;
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

    NetworkCostFlow network(N+2);

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
        network.addEdge(from, to, NetworkCostFlow::INF, 1);
        // unfortunately
        // network.addEdge(to, from, NetworkCostFlow::INF, 1);
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

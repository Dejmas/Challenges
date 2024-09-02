#include <vector>
#include <deque>
#include <iostream>
#include <set>

// This implementation is modified Edmonds-Karp for computin max flow.
// Edges of negative costs are allowed.
// Algorithm to find the shortest path (in terms of costs) is called Shortest Path Faster Algorithm (SPFA), because of negative costs. 
// No parallel edges, each pair of node can only have edge in one direction or none.
class NetworkCostFlow {
    public: 
        NetworkCostFlow(size_t noNodes);
        void addEdge(int a, int b, long cap, long cost);
        long minCostFlow(int source, int target, long flowLimit);

        static long INF;
        // for task
        auto getResidueNetwork() const { return m_residues; }

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


// Tested
// CSES Task Assignment https://cses.fi/problemset/task/2129
bool Task_Assignment() {
    int N;
    if (!(std::cin >> N)) {
        return false;
    }

    // Build flow network
    const int source = 0;
    const int target = 2*N+1;

    NetworkCostFlow network(2*N+2);

    for (int i = 1; i <= N; i ++) {
        int employe = i;
        int task = N + i;
        network.addEdge(source, employe, 1, 0);
        network.addEdge(task, target, 1, 0);
    }

    for (int employee = 1; employee <= N; employee ++) {
        for (int task = N + 1; task <= 2*N; task ++) {
            int cost;
            if (!(std::cin >> cost)) {
                return false;
            }
            network.addEdge(employee, task, 1, cost);
        }
    }
    // Looking for the cost of flow N from source to target.
    auto cost = network.minCostFlow(source, target, N);
    std::cout << cost << "\n";
    // Show employee and task assignments
    auto residue = network.getResidueNetwork();
    for (int employee = 1; employee <= N; employee ++) {
        for (int task = N+1; task <= N+N; task ++) {
            if (residue[employee][task] == 0) {
                std::cout << employee << " " << task-N << "\n";
            }
        }
    }
    return true;
}

int main () {
    // return Task_Assignment();
    return Task_Assignment() ? 0 : 1;
}
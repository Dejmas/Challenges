#include <vector>
#include <deque>
#include <iostream>
#include <set>

class NetworkCostFlow {
    public: 
        NetworkCostFlow(size_t noNodes);
        void addEdge(int a, int b, long cap);
        long maxFlow(int source, int target);

        inline static long INF = 1e18; 

    private:
        long augmentingPath(int source, int target, std::vector<int> & parent) const;

        std::vector<std::vector<int>> m_adjacent;
        std::vector<std::vector<long>> m_residues;
        std::set<std::pair<int, int>> m_existingEdges;
};

NetworkCostFlow::NetworkCostFlow(size_t noNodes) 
: m_adjacent(noNodes)
, m_residues(noNodes, std::vector<long>(noNodes, 0)) {
}

void NetworkCostFlow::addEdge(int a, int b, long cap) {
    if (!m_existingEdges.contains({a,b})) {
        m_adjacent[a].push_back(b);
        m_adjacent[b].push_back(a);
        m_residues[a][b] = cap;
        m_existingEdges.insert({a,b});
    } else {
        m_residues[a][b] += cap;
    }
}

long NetworkCostFlow::maxFlow(int source, int target) {
    long flow = 0;
    std::vector<int>parent(m_adjacent.size());
    long path_flow;
    while ((path_flow = augmentingPath(source, target, parent)) > 0 ) {
        flow += path_flow;
        //std::cout << "augmenting path " << path_flow << ": ";
        for (int current = target; current != source; ) {
            int prev = parent[current];
            m_residues[prev][current] -= path_flow;
            m_residues[current][prev] += path_flow;
            //std::cout << current << " -> " << prev << " ";
            current = prev;
        }
        //std::cout << "\n";
    }

    return flow;
}

long NetworkCostFlow::augmentingPath(int source, int target, std::vector<int> & parent) const{
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

int main () {
    int N, edges;
    
    if (!(std::cin >> N >> edges)) {
        std::cerr << "Invalid input.\n";
        return 1;
    }
    
    NetworkCostFlow network(N+1);
    while (edges -- > 0) {
        int from, to; long flow;
        if (!(std::cin >> from >> to >> flow)) {
            std::cerr << "Invalid input.\n";
            return 2;
        }
        network.addEdge(from, to, flow);
    } 
    long flow = network.maxFlow(1, N);
    std::cout << flow << "\n";
}


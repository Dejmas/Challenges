#include <vector>
#include <deque>
#include <iostream>
#include <set>

class NetworkFlow {
    public: 
        NetworkFlow(size_t noNodes);
        void addEdge(int a, int b, long cap);
        long maxFlow(int source, int target);

        inline static long INF = 1e18; 

    private:
        long augmentingPath(int source, int target, std::vector<int> & parent) const;

        std::vector<std::vector<int>> m_adjacent;
        std::vector<std::vector<long>> m_residues;
        std::set<std::pair<int, int>> m_existingEdges;
};

NetworkFlow::NetworkFlow(size_t noNodes) 
: m_adjacent(noNodes)
, m_residues(noNodes, std::vector<long>(noNodes, 0)) {
}

void NetworkFlow::addEdge(int a, int b, long cap) {
    if (!m_existingEdges.contains({a,b})) {
        m_adjacent[a].push_back(b);
        m_adjacent[b].push_back(a);
        m_residues[a][b] = cap;
        m_existingEdges.insert({a,b});
    } else {
        m_residues[a][b] += cap;
    }
}

long NetworkFlow::maxFlow(int source, int target) {
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

long NetworkFlow::augmentingPath(int source, int target, std::vector<int> & parent) const{
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


void testMaxFlow () {
    NetworkFlow network(6);
    network.addEdge(0, 1, 7);
    network.addEdge(1, 2, 5);
    network.addEdge(2, 5, 8);
    network.addEdge(1, 3, 3);
    network.addEdge(0, 4, 4);
    network.addEdge(4, 1, 3);
    network.addEdge(4, 3, 2);
    network.addEdge(3, 2, 3);
    network.addEdge(3, 5, 5);

    auto flow = network.maxFlow(0, 5);
    std::cout << "Max flow is " << flow << "\n";    
}

int main () {
    testMaxFlow();
}
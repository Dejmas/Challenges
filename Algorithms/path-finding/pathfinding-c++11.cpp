#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

#include <vector>

class Graph {
    public:

        virtual bool addEdge(int a, int b, int weight) = 0; 
        virtual void findPaths(int start, std::vector<long> & distance, std::vector<int> & parent) const = 0;
        
        static long INF;

};

long Graph::INF =  1e18;

#include <cassert>
#include <type_traits>

template <class GraphType>
void basic_test() {
    static_assert(std::is_base_of<Graph, GraphType>::value == true, "GraphType needs to be derived from Graph");
    GraphType graph {6};
    graph.addEdge(0, 1, 5);
    graph.addEdge(0, 2, 35);
    graph.addEdge(0, 3, 40);
    graph.addEdge(1, 3, 20);
    graph.addEdge(1, 4, 25);
    graph.addEdge(2, 4, 30);
    graph.addEdge(2, 5, 30);
    graph.addEdge(3, 5, 20);
    graph.addEdge(4, 5, 25);

    std::vector<long> dist;
    std::vector<int> parent;
    graph.findPaths(0, dist, parent);

    assert(dist[0] == 0 && parent[0] == -1);
    assert(dist[1] == 5 && parent[1] == 0);
    assert(dist[2] == 35 && parent[2] == 0);
    assert(dist[3] == 25 && parent[3] == 1);
    assert(dist[4] == 30 && parent[4] == 1);
    assert(dist[5] == 45 && parent[5] == 3);
}

template <class GraphType>
void negative_edge_test() {
    static_assert(std::is_base_of<Graph, GraphType>::value == true, "GraphType needs to be derived from Graph");
    GraphType graph {6};
    graph.addEdge(0, 1, 5);
    graph.addEdge(0, 2, 35);
    graph.addEdge(0, 3, 40);
    graph.addEdge(1, 3, 20);
    graph.addEdge(1, 4, 25);
    graph.addEdge(2, 4, -30);
    graph.addEdge(2, 5, 30);
    graph.addEdge(3, 5, 20);
    graph.addEdge(4, 5, 25);

    std::vector<long> dist;
    std::vector<int> parent;
    graph.findPaths(0, dist, parent);

    assert(dist[0] == 0 && parent[0] == -1);
    assert(dist[1] == 5 && parent[1] == 0);
    assert(dist[2] == 35 && parent[2] == 0);
    assert(dist[3] == 25 && parent[3] == 1);
    assert(dist[4] == 5 && parent[4] == 2);
    assert(dist[5] == 30 && parent[5] == 4);
}

template <class GraphType>
void negative_cycle_test() {
    static_assert(std::is_base_of<Graph, GraphType>::value == true, "GraphType needs to be derived from Graph");
    GraphType graph{5};
    graph.addEdge(0, 1, 1);
    graph.addEdge(1, 2, -1);
    graph.addEdge(2, 3, -1);
    graph.addEdge(3, 1, -1);
    graph.addEdge(3, 4, 1);
    try {
        std::vector<long> dist;
        std::vector<int> parent;
        graph.findPaths(0, dist, parent);
        assert("Exception should be thrown" == nullptr);
    } catch (const std::exception & expt) {
    }
}

template <class GraphType>
void zero_sum_cycle_test() {
    static_assert(std::is_base_of<Graph, GraphType>::value == true, "GraphType needs to be derived from Graph");
    GraphType graph{5};
    graph.addEdge(0, 1, 1);
    graph.addEdge(1, 2, -1);
    graph.addEdge(2, 3, -1);
    graph.addEdge(3, 1, 2);
    graph.addEdge(3, 4, 1);
    try {
        std::vector<long> dist;
        std::vector<int> parent;
        graph.findPaths(0, dist, parent);
        assert(dist[ 0 ] == 0 && parent[ 0 ] == -1);
        assert(dist[ 1 ] == 1 && parent[ 1 ] == 0);
        assert(dist[ 2 ] == 0 && parent[ 2 ] == 1);
        assert(dist[ 3 ] == -1 && parent[ 3 ] == 2);
        assert(dist[ 4 ] == 0 && parent[ 4 ] == 3);
    } catch (const std::exception & expt) {
        assert("Exception should not be thrown" == nullptr);
    }
}




class BellmanFord : public Graph {
    public:
        BellmanFord (size_t N);
        bool addEdge(int a, int b, int weight) override; 
        void findPaths(int start, std::vector<long> & distance, std::vector<int> & parent) const override;

    private:

        std::vector<std::vector<std::pair<int,long>>> mAdjacent;
};

BellmanFord::BellmanFord(size_t N): mAdjacent(N) {}

bool BellmanFord::addEdge(int a, int b, int weight) {
    mAdjacent[a].push_back({b, weight});
    return true;
}

void BellmanFord::findPaths(int start, std::vector<long> & dist, std::vector<int> & parent) const {
    size_t const N = mAdjacent.size();
    dist.assign(N, INF);
    parent.assign(N, -1);
    dist[start] = 0;
    size_t sweeps = 0;
    for (bool change = true; change; sweeps ++) {
        if (sweeps == N) {
            throw std::invalid_argument("Some negative cycle was detected");
        }
        change = false;
        for (size_t u = 0; u < N; ++ u) {
            for (auto x : mAdjacent[u]) {
                auto v = x.first;
                auto weight = x.second;
                if (dist[u] != INF && dist[v] > dist[u] + weight) {
                    dist[v] = dist[u] + weight;
                    parent[v] = u;
                    change = true;
                }
            }
        }
    }
}



#include <queue>
class Dijkstra : public Graph {
    public:
        Dijkstra (size_t N);
        bool addEdge(int a, int b, int weight) override; 
        void findPaths(int start, std::vector<long> & distance, std::vector<int> & parent) const override;

    private:

        std::vector<std::vector<std::pair<int,long>>> mAdjacent;
};

Dijkstra::Dijkstra(size_t N): mAdjacent(N) {}

bool Dijkstra::addEdge(int a, int b, int weight) {
    mAdjacent[a].push_back({b, weight});
    return true;
}

void Dijkstra::findPaths(int start, std::vector<long> & dist, std::vector<int> & parent) const {
    size_t const N = mAdjacent.size();
    dist.assign(N, INF);
    parent.assign(N, -1);
    dist[start] = 0;
    std::priority_queue<
        std::pair<long, int>,
        std::vector<std::pair<long, int>>,
        std::greater<std::pair<long, int>>> queue;
    queue.push({0, start});
    std::vector<bool> visited(N, false);
    while (!queue.empty()) {
        auto current = queue.top().second;
        queue.pop();
        
        if (visited[current]) continue;
        visited[current] = true;

        for (auto x : mAdjacent[current]) {
            int next = x.first;
            long weight = x.second;
            if (dist[next] > dist[current] + weight) {
                dist[next] = dist[current] + weight;
                parent[next] = current;
                queue.push({dist[next], next});
            }
        }
    }
}


/// @brief Shortest Path Fast Algorithm improved Bellman-Ford
class SPFA : public Graph {
    public:
        SPFA (size_t N);
        bool addEdge(int a, int b, int weight) override; 
        void findPaths(int start, std::vector<long> & distance, std::vector<int> & parent) const override;

    private:

        std::vector<std::vector<std::pair<int,long>>> mAdjacent;
};

SPFA::SPFA(size_t N): mAdjacent(N) {}

bool SPFA::addEdge(int a, int b, int weight) {
    mAdjacent[a].push_back({b, weight});
    return true;
}

void SPFA::findPaths(int start, std::vector<long> & dist, std::vector<int> & parent) const {
    size_t const N = mAdjacent.size();
    dist.assign(N, INF);
    parent.assign(N, -1);
    std::vector<bool> inqueue(N, false);
    std::vector<size_t> count(N, 0);
    std::deque<int> queue = {start};
    inqueue[start] = true;
    dist[start] = 0;

    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop_front();        
        inqueue[current] = false;

        for (auto edge : mAdjacent[current]) {
            auto next = edge.first;
            auto weight = edge.second;
            if (dist[next] > dist[current] + weight) {
                dist[next] = dist[current] + weight;
                parent[next] = current;
                if (!inqueue[next]) {
                    queue.push_back(next);
                    inqueue[next] = true;
                    if (++ count[next] > N) {
                        throw std::invalid_argument("Some negative cycle was detected");
                    }
                }
            }
        }
    }
}

void testing() {
    basic_test<BellmanFord>();
    negative_edge_test<BellmanFord>();
    negative_cycle_test<BellmanFord>();
    zero_sum_cycle_test<BellmanFord>();

    basic_test<Dijkstra>();
    // negative_edge_test<Dijkstra>();
    // negative_cycle_test<Dijkstra>();

    basic_test<SPFA>();
    negative_edge_test<SPFA>();
    negative_cycle_test<SPFA>();
    zero_sum_cycle_test<SPFA>();
}

int main () {
    testing();

    int N;
    if (!(std::cin>>N)) { 
        std::cerr << "Invalid input\n";
        return 1;
    }

    SPFA graph {static_cast<size_t>(N)};

    for (int i = 0; i < N; i ++) {
        for (int j = 0; j < i; j ++) {
            int cost;
            if ((std::cin >> std::ws).peek() == 'x') {
                char x; std::cin >> x;
                continue; 
            }
            if (!(std::cin >> cost)) {
                std::cerr << "Invalid input\n";
                return 2;
            }
            graph.addEdge(i, j, cost);
            graph.addEdge(j, i, cost);
        }
    }

    std::vector<long> dist;
    std::vector<int> parent;
    graph.findPaths(0, dist, parent);
    //for (int i = 0; i < N; i ++) {
    //   std::cout << "dist[ " << i << " ] = " << dist[i] << ", from " << parent[i] << "\n";
    //}
    std::cout << *std::max_element(dist.begin(), dist.end()) << "\n";
}

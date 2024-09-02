#include <vector>
#include <deque>
#include <iostream>
#include <set>
#include <cmath>

const double eps = 1e-12;

class FlowNetwork {
public:
    virtual void addEdge(int a, int b, long cap) = 0;
    virtual long maxFlow(int source, int target) = 0;
    virtual ~FlowNetwork() = default;

    inline static long INF = 1e18; 

};

class Dinitz : public FlowNetwork {
    public: 
        Dinitz(size_t noNodes);
        void addEdge(int a, int b, long cap) override;
        long maxFlow(int source, int target) override;

        inline static long INF = 1e18; 

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
    m_adjacent[a].push_back(m ++);
    m_edges.push_back(Edge{b, a, 0, 0, 0});
    m_adjacent[b].push_back(m);
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
    if (distance[target] == INF) return false;

    return true;
}

long Dinitz::dfs_and_cleanup(int node, int target, long flow) {
    if (!flow || node == target) {
        return flow;
    }
    for (int i = m_clean_adjacent[node].size() -1; i >= 0; i --) {
        int idx = m_clean_adjacent[node][i];
        Edge edge = m_edges[idx];
        long returned_flow = dfs_and_cleanup(edge.to, target, std::min(flow, edge.residue()));
        if (returned_flow) {
            m_edges[idx].flow += returned_flow;
            m_edges[idx ^ 1].flow -= returned_flow;
            return returned_flow;
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

struct Coord {
    int x;
    int y;
};

inline long distance(const Coord & a, const Coord & b) {
    return abs(a.x - b.x)  + abs(a.y - b.y);
}

void taxi() {
    int peopleCnt, carsCnt;
    long speed, time;
    std::cin >> peopleCnt >> carsCnt >> speed >> time;
    std::vector<Coord> people(peopleCnt), cars(carsCnt);
    for (auto & p : people) std::cin >> p.x >> p.y;
    for (auto & c : cars) std::cin >> c.x >> c.y;

    const int source = 0;
    const int sink = 1 + peopleCnt + carsCnt;

    Dinitz network(peopleCnt + carsCnt + 2);
    for (size_t j = 0; j < cars.size(); ++ j) {
        network.addEdge(1 + peopleCnt + j, sink, 1);
    }

    for (size_t i = 0; i < people.size(); ++ i) {
        network.addEdge(source, 1 + i, 1);
        for (size_t j = 0; j < cars.size(); ++ j) {
            if (distance(people[i], cars[j]) * 200L <= speed * time) {
                network.addEdge(1 + i, 1 + peopleCnt + j, 1);
            }
        }
    }

    std::cout << network.maxFlow(source, sink) << "\n";
}

int main()
{
    int testCases;
    std::cin >> testCases;

    while (testCases --) {
        taxi();
    }
 
    return 0;
}
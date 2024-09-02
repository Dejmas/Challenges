#pragma once


class FlowNetwork {
public:
    virtual void addEdge(int a, int b, long cap) = 0;
    virtual long maxFlow(int source, int target) = 0;
    virtual ~FlowNetwork() = default;

    inline static long INF = 1e18; 

};



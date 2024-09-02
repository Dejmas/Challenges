#pragma once 

#include "flow-network.h"

#include <cassert>
#include <concepts>

template <class GraphType> requires std::derived_from<GraphType, FlowNetwork>  
void testMaxFlow () {
    GraphType network(6);
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
    assert(flow == 10);
}

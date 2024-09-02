#pragma once

#include <vector>

class Graph {
    public:

        virtual bool addEdge(int a, int b, int weight) = 0; 
        virtual void findPaths(int start, std::vector<long> & distance, std::vector<int> & parent) const = 0;
        
        inline static long INF = 1e18;

};

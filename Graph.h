//
// Created by nivh2 on 2/16/2021.
//

#ifndef UNTITLED2_GRAPH_H
#define UNTITLED2_GRAPH_H


#include <cstdint>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;

struct node {
    uint32_t vec;
    unordered_set<struct node*> edges;
} node_t;

class Graph {
    unordered_map<uint32_t, node*> map;
    unordered_set<node> nodes;
public:
    Graph(int len);
};

#endif //UNTITLED2_GRAPH_H

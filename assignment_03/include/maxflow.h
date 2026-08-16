#ifndef MAXFLOW_H
#define MAXFLOW_H

#include "../../assignment_01/include/csr.h"
#include <string>
#include <tuple>
#include <vector>

struct MaxflowResult
{
    double max_flow = 0.0;
    std::vector<int> source_side;
    std::vector<int> sink_side;
    std::vector<std::tuple<int, int, double>> cut_edges;
};

bool read_maxflow_input(const std::string &path, AdjacencyList &g, int &sink, std::string &error);

MaxflowResult dinic_maxflow(const CSR &csr, int source, int sink);

#endif

#ifndef GRAPH_ALGORITHMS_H
#define GRAPH_ALGORITHMS_H

#include "csr.h"
#include <vector>
#include <limits>

// --- BFS -----------------------------------------------------------------

struct BFSResult
{
    std::vector<int> traversal;
    std::vector<long long> distance;
};
BFSResult bfs_csr(const CSR &csr, int source);

// --------DFS----------------------------------------------------

std::vector<int> dfs_csr(const CSR &csr, int source);

// --- SSSP (Dijkstra) ---------------------------------------------------

struct SSSPResult
{
    std::vector<double> distance;
};
SSSPResult sssp_dijkstra_csr(const CSR &csr, int source);

#endif

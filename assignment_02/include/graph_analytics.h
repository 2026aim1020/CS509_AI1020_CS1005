#ifndef GRAPH_ANALYTICS_H
#define GRAPH_ANALYTICS_H

#include "../../assignment_01/include/csr.h"
#include <array>
#include <vector>

// --- Triangle Counting -----------------------------------------------------
struct TriangleResult
{
    long long total_triangles = 0;
    std::vector<std::array<int, 3>> triangles; // only populated when collect_list is true
};
TriangleResult count_triangles(const CSR &csr, bool collect_list);

// --- Betweenness Centrality (Brandes' algorithm) --------------------------
struct CentralityResult
{
    std::vector<double> centrality; // size V
};
CentralityResult betweenness_centrality(const CSR &csr);

// --- Connected Components --------------------------------------------------
struct ComponentsResult
{
    int num_components = 0;
    std::vector<int> component_id; // size V
};
ComponentsResult connected_components(const CSR &csr);

#endif

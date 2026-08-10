#include "../include/graph_analytics.h"
#include "../../assignment_01/include/graph_algorithms.h" // Assignment 1's bfs_csr()

#include <algorithm>
#include <queue>

// --- Triangle Counting ------------------------------------------------------

TriangleResult count_triangles(const CSR &csr, bool collect_list)
{
    TriangleResult res;

    // Sort a private copy of each vertex's neighbour list so common-neighbour checks can use
    // binary search instead of a linear scan, per the spec's suggestion.
    std::vector<std::vector<int>> sorted_adj(csr.V);
    for (int u = 0; u < csr.V; ++u)
    {
        int start = csr.row_ptr[u], end = csr.row_ptr[u + 1];
        sorted_adj[u].assign(csr.col_idx.begin() + start, csr.col_idx.begin() + end);
        std::sort(sorted_adj[u].begin(), sorted_adj[u].end());
    }

    long long raw_count = 0;
    for (int u = 0; u < csr.V; ++u)
    {
        const auto &nu = sorted_adj[u];
        for (size_t i = 0; i < nu.size(); ++i)
        {
            int v = nu[i];
            for (size_t j = i + 1; j < nu.size(); ++j)
            {
                int w = nu[j];
                // Common-neighbour check
                if (std::binary_search(sorted_adj[v].begin(), sorted_adj[v].end(), w))
                {
                    ++raw_count;
                    // Each triangle {u,v,w} (u<v<w here since nu is sorted
                    // and i<j) is only recorded once, at its smallest vertex.
                    if (collect_list && u < v)
                    {
                        res.triangles.push_back({u, v, w});
                    }
                }
            }
        }
    }

    // Each triangle was discovered once at each of its three vertices.
    res.total_triangles = raw_count / 3;
    return res;
}

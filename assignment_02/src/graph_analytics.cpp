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

// --- Betweenness Centrality (Brandes' algorithm) ---------------------------
CentralityResult betweenness_centrality(const CSR &csr)
{
    const int V = csr.V;
    CentralityResult res;
    res.centrality.assign(V, 0.0);

    std::vector<std::vector<int>> preds(V);
    std::vector<long long> sigma(V);
    std::vector<int> dist(V);
    std::vector<int> order;
    order.reserve(V);
    std::vector<double> delta(V);

    for (int s = 0; s < V; ++s)
    {
        for (int i = 0; i < V; ++i)
        {
            preds[i].clear();
            sigma[i] = 0;
            dist[i] = -1;
        }
        order.clear();

        sigma[s] = 1;
        dist[s] = 0;
        std::queue<int> q;
        q.push(s);
        while (!q.empty())
        {
            int v = q.front();
            q.pop();
            order.push_back(v);
            for (int idx = csr.row_ptr[v]; idx < csr.row_ptr[v + 1]; ++idx)
            {
                int w = csr.col_idx[idx];
                if (dist[w] < 0)
                {
                    dist[w] = dist[v] + 1;
                    q.push(w);
                }
                if (dist[w] == dist[v] + 1)
                {
                    sigma[w] += sigma[v];
                    preds[w].push_back(v);
                }
            }
        }
        std::fill(delta.begin(), delta.end(), 0.0);
        for (int i = static_cast<int>(order.size()) - 1; i >= 0; --i)
        {
            int w = order[i];
            for (int v : preds[w])
            {
                delta[v] += (static_cast<double>(sigma[v]) / static_cast<double>(sigma[w])) * (1.0 + delta[w]);
            }
            if (w != s)
                res.centrality[w] += delta[w];
        }
    }
    for (double &c : res.centrality)
        c /= 2.0;

    return res;
}

// --- Connected Components ---------------------------------------------------
ComponentsResult connected_components(const CSR &csr)
{
    ComponentsResult res;
    res.component_id.assign(csr.V, -1);

    int comp_id = 0;
    for (int start = 0; start < csr.V; ++start)
    {
        if (res.component_id[start] != -1)
            continue;
        BFSResult bfs = bfs_csr(csr, start);
        for (int v : bfs.traversal)
        {
            res.component_id[v] = comp_id;
        }
        ++comp_id;
    }
    res.num_components = comp_id;
    return res;
}

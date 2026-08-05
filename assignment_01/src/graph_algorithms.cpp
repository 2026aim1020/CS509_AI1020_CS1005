#include "graph_algorithms.h"
#include <queue>
#include <utility>
#include <functional>

BFSResult bfs_csr(const CSR &csr, int source)
{
    BFSResult res;
    res.distance.assign(csr.V, -1);
    if (source < 0 || source >= csr.V)
        return res;

    std::vector<char> visited(csr.V, 0);
    std::queue<int> q;

    visited[source] = 1;
    res.distance[source] = 0;
    q.push(source);

    while (!q.empty())
    {
        int u = q.front();
        q.pop();
        res.traversal.push_back(u);

        for (int idx = csr.row_ptr[u]; idx < csr.row_ptr[u + 1]; ++idx)
        {
            int v = csr.col_idx[idx];
            if (!visited[v])
            {
                visited[v] = 1;
                res.distance[v] = res.distance[u] + 1;
                q.push(v);
            }
        }
    }
    return res;
}

std::vector<int> dfs_csr(const CSR &csr, int source)
{
    std::vector<int> traversal;
    if (source < 0 || source >= csr.V)
        return traversal;

    std::vector<char> visited(csr.V, 0);
    std::vector<int> stack;
    stack.push_back(source);

    while (!stack.empty())
    {
        int u = stack.back();
        stack.pop_back();
        if (visited[u])
            continue;

        visited[u] = 1;
        traversal.push_back(u);

        for (int idx = csr.row_ptr[u + 1] - 1; idx >= csr.row_ptr[u]; --idx)
        {
            int v = csr.col_idx[idx];
            if (!visited[v])
                stack.push_back(v);
        }
    }
    return traversal;
}

SSSPResult sssp_dijkstra_csr(const CSR &csr, int source)
{
    SSSPResult res;
    const double INF = std::numeric_limits<double>::infinity();
    res.distance.assign(csr.V, INF);
    if (source < 0 || source >= csr.V)
        return res;

    res.distance[source] = 0.0;

    using P = std::pair<double, int>; // (distance, vertex)
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    pq.push({0.0, source});

    std::vector<char> finalized(csr.V, 0);

    while (!pq.empty())
    {
        auto [d, u] = pq.top();
        pq.pop();
        if (finalized[u])
            continue;
        finalized[u] = 1;

        for (int idx = csr.row_ptr[u]; idx < csr.row_ptr[u + 1]; ++idx)
        {
            int v = csr.col_idx[idx];
            double w = csr.values[idx]; // must be > 0, per assignment spec
            double cand = res.distance[u] + w;
            if (cand < res.distance[v])
            {
                res.distance[v] = cand;
                pq.push({cand, v});
            }
        }
    }
    return res;
}

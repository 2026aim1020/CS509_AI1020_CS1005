#include "../include/maxflow.h"

#include <algorithm>
#include <fstream>
#include <limits>
#include <queue>
#include <sstream>

bool read_maxflow_input(const std::string &path, AdjacencyList &g, int &sink, std::string &error)
{
    if (!read_adjacency_list(path, g))
    {
        error = "could not read/parse file";
        return false;
    }
    sink = -1;
    std::ifstream f(path);
    std::string line;
    while (std::getline(f, line))
    {
        std::istringstream ss(line);
        std::string tag;
        if (ss >> tag && tag == "SINK")
        {
            ss >> sink;
            break;
        }
    }

    if (sink < 0 || sink >= g.V)
    {
        error = "missing or invalid SINK";
        return false;
    }
    if (g.source < 0 || g.source >= g.V)
    {
        error = "invalid SOURCE";
        return false;
    }
    if (g.source == sink)
    {
        error = "source equals sink";
        return false;
    }
    for (const auto &lst : g.adj)
        for (const auto &e : lst)
            if (e.weight < 0)
            {
                error = "negative capacity";
                return false;
            }

    return true;
}

namespace
{
    struct FlowEdge
    {
        int to;
        double cap;
        int rev;
    };

    struct Dinic
    {
        int n;
        std::vector<std::vector<FlowEdge>> graph;
        std::vector<int> level, it;

        explicit Dinic(int n_) : n(n_), graph(n_), level(n_), it(n_) {}

        void add_edge(int u, int v, double cap)
        {
            graph[u].push_back({v, cap, static_cast<int>(graph[v].size())});
            graph[v].push_back({u, 0.0, static_cast<int>(graph[u].size()) - 1});
        }

        bool bfs(int s, int t)
        {
            level.assign(n, -1);
            std::queue<int> q;
            level[s] = 0;
            q.push(s);
            while (!q.empty())
            {
                int u = q.front();
                q.pop();
                for (const auto &e : graph[u])
                {
                    if (e.cap > 1e-12 && level[e.to] < 0)
                    {
                        level[e.to] = level[u] + 1;
                        q.push(e.to);
                    }
                }
            }
            return level[t] >= 0;
        }

        double dfs(int u, int t, double f)
        {
            if (u == t)
                return f;
            for (int &i = it[u]; i < static_cast<int>(graph[u].size()); ++i)
            {
                FlowEdge &e = graph[u][i];
                if (e.cap > 1e-12 && level[u] < level[e.to])
                {
                    double d = dfs(e.to, t, std::min(f, e.cap));
                    if (d > 1e-12)
                    {
                        e.cap -= d;
                        graph[e.to][e.rev].cap += d;
                        return d;
                    }
                }
            }
            return 0.0;
        }

        double max_flow(int s, int t)
        {
            double flow = 0.0;
            while (bfs(s, t))
            {
                it.assign(n, 0);
                double f;
                while ((f = dfs(s, t, std::numeric_limits<double>::infinity())) > 1e-12)
                    flow += f;
            }
            return flow;
        }
    };
}

MaxflowResult dinic_maxflow(const CSR &csr, int source, int sink)
{
    MaxflowResult result;
    int V = csr.V;
    Dinic dinic(V);
    for (int u = 0; u < V; ++u)
        for (int k = csr.row_ptr[u]; k < csr.row_ptr[u + 1]; ++k)
            dinic.add_edge(u, csr.col_idx[k], csr.values[k]);

    result.max_flow = dinic.max_flow(source, sink);
    std::vector<bool> visited(V, false);
    std::queue<int> q;
    visited[source] = true;
    q.push(source);
    while (!q.empty())
    {
        int u = q.front();
        q.pop();
        for (const auto &e : dinic.graph[u])
        {
            if (e.cap > 1e-9 && !visited[e.to])
            {
                visited[e.to] = true;
                q.push(e.to);
            }
        }
    }

    for (int v = 0; v < V; ++v)
        (visited[v] ? result.source_side : result.sink_side).push_back(v);

    for (int u = 0; u < V; ++u)
    {
        if (!visited[u])
            continue;
        for (int k = csr.row_ptr[u]; k < csr.row_ptr[u + 1]; ++k)
        {
            int v = csr.col_idx[k];
            if (!visited[v])
                result.cut_edges.push_back({u, v, csr.values[k]});
        }
    }

    return result;
}

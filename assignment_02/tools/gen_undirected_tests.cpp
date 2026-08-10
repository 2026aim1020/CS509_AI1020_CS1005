// Usage:
//   ./gen_undirected_tests <out_dir>
//
// Produces, using the required size lists:
//   tc_<V>.txt  for V in {10, 100, 10000, 50000, 100000}   (Triangle Counting)
//   cc_<V>.txt  for V in {10, 100, 10000, 50000, 100000}   (Connected Components,
//                                                            same connected graphs as TC)
//   bc_<V>.txt  for V in {10, 100, 1000, 5000, 10000}      (Betweenness Centrality,
//                                                            smaller scale per spec)
//
// TC/CC graphs: spanning path (guarantees connectivity, matching the
// "every vertex must be assigned to a component" requirement), plus extra random edges AND a few explicit triangles
// stitched 
//
// A separate cc_disconnected file with isolated vertices and multiple
// components is written 

#include <cstdio>
#include <string>
#include <vector>
#include <set>
#include <random>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(path) mkdir(path, 0777)
#endif

static const std::vector<long long> TC_CC_SIZES = {10, 100, 10000, 50000, 100000};
static const std::vector<long long> BC_SIZES = {10, 100, 1000, 5000, 10000};
static const int AVG_DEGREE = 4;

// Builds a connected undirected graph with a decent number of triangles:
// spanning path + extra random edges
static long long build_graph(long long V, std::mt19937 &rng,
                             std::vector<std::vector<long long>> &adj)
{
    std::set<std::pair<long long, long long>> edges;

    for (long long u = 0; u + 1 < V; ++u)
        edges.insert({u, u + 1});
    for (long long u = 0; u + 2 < V; u += 2)
        edges.insert({u, u + 2}); // closes triangles with the path

    long long extra = (V * AVG_DEGREE) / 2;
    long long target = static_cast<long long>(edges.size()) + extra;
    long long attempts = 0, max_attempts = extra * 10 + 10;
    std::uniform_int_distribution<long long> dist(0, V - 1);

    while (static_cast<long long>(edges.size()) < target && attempts < max_attempts)
    {
        ++attempts;
        long long u = dist(rng), v = dist(rng);
        if (u == v)
            continue;
        long long a = std::min(u, v), b = std::max(u, v);
        edges.insert({a, b});
    }

    adj.assign(V, {});
    for (const auto &e : edges)
    {
        adj[e.first].push_back(e.second);
        adj[e.second].push_back(e.first);
    }
    return static_cast<long long>(edges.size());
}

static void write_graph(const std::string &path, long long V,
                        const std::vector<std::vector<long long>> &adj, long long E)
{
    std::ofstream f(path);
    f << V << " " << E << "\n";
    for (long long u = 0; u < V; ++u)
    {
        const auto &nb = adj[u];
        f << u << " " << nb.size();
        for (long long n : nb)
            f << " " << n;
        f << "\n";
    }
}

int main(int argc, char **argv)
{
    std::string out_dir = (argc > 1) ? argv[1] : ".";
    MKDIR(out_dir.c_str());

    std::mt19937 rng_tc(11);
    for (long long V : TC_CC_SIZES)
    {
        std::vector<std::vector<long long>> adj;
        long long E = build_graph(V, rng_tc, adj);

        std::ostringstream tc_name, cc_name;
        tc_name << out_dir << "/tc_" << V << ".txt";
        cc_name << out_dir << "/cc_" << V << ".txt";

        write_graph(tc_name.str(), V, adj, E);
        write_graph(cc_name.str(), V, adj, E); // same connected graph, CC-named copy

        std::printf("wrote %s and %s (V=%lld, E=%lld)\n",
                    tc_name.str().c_str(), cc_name.str().c_str(), V, E);
    }

    std::mt19937 rng_bc(22);
    for (long long V : BC_SIZES)
    {
        std::vector<std::vector<long long>> adj;
        long long E = build_graph(V, rng_bc, adj);

        std::ostringstream bc_name;
        bc_name << out_dir << "/bc_" << V << ".txt";
        write_graph(bc_name.str(), V, adj, E);
        std::printf("wrote %s (V=%lld, E=%lld)\n", bc_name.str().c_str(), V, E);
    }

    return 0;
}

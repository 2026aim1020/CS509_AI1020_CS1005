#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(path) mkdir(path, 0777)
#endif

static const std::vector<long long> SIZES = {10, 100, 1000, 10000, 50000, 100000};
static const int AVG_DEGREE = 4;
static const int MIN_CAP = 1;
static const int MAX_CAP = 50;

static long long build_maxflow_graph(long long V, std::mt19937 &rng,
                                     std::vector<std::vector<std::pair<long long, int>>> &adj,
                                     long long &source, long long &sink)
{
    source = 0;
    sink = V - 1;

    std::uniform_int_distribution<int> cdist(MIN_CAP, MAX_CAP);
    std::set<std::pair<long long, long long>> chosen;
    std::vector<std::pair<std::pair<long long, long long>, int>> edge_list;

    auto add_edge = [&](long long u, long long v) -> bool
    {
        if (u == v)
            return false;
        if (chosen.count({u, v}))
            return false;
        chosen.insert({u, v});
        edge_list.push_back({{u, v}, cdist(rng)});
        return true;
    };

    std::vector<long long> mid;
    for (long long i = 1; i < V - 1; ++i)
        mid.push_back(i);
    std::shuffle(mid.begin(), mid.end(), rng);

    long long prev = source;
    for (long long v : mid)
        if (add_edge(prev, v))
            prev = v;
    add_edge(prev, sink);

    long long extra = V * AVG_DEGREE;
    long long target = static_cast<long long>(edge_list.size()) + extra;
    long long attempts = 0;
    long long max_attempts = extra * 10 + 10;
    std::uniform_int_distribution<long long> dist(0, V - 1);

    while (static_cast<long long>(edge_list.size()) < target && attempts < max_attempts)
    {
        ++attempts;
        long long u = dist(rng);
        long long v = dist(rng);
        if (u == sink || v == source)
            continue;
        add_edge(u, v);
    }

    adj.assign(V, {});
    for (const auto &e : edge_list)
        adj[e.first.first].push_back({e.first.second, e.second});

    return static_cast<long long>(edge_list.size());
}

static void write_maxflow(const std::string &path, long long V,
                          const std::vector<std::vector<std::pair<long long, int>>> &adj,
                          long long E, long long source, long long sink)
{
    std::ofstream f(path);
    f << V << " " << E << "\n";
    for (long long u = 0; u < V; ++u)
    {
        const auto &nb = adj[u];
        f << u << " " << nb.size();
        for (const auto &[v, c] : nb)
            f << " " << v << " " << c;
        f << "\n";
    }
    f << "SOURCE " << source << "\n";
    f << "SINK " << sink << "\n";
}

int main(int argc, char **argv)
{
    std::string out_dir = (argc > 1) ? argv[1] : ".";
    MKDIR(out_dir.c_str());

    std::mt19937 rng(65);

    for (long long V : SIZES)
    {
        std::vector<std::vector<std::pair<long long, int>>> adj;
        long long source, sink;
        long long E = build_maxflow_graph(V, rng, adj, source, sink);

        std::ostringstream name;
        name << out_dir << "/maxflow_" << V << ".txt";

        write_maxflow(name.str(), V, adj, E, source, sink);
        std::printf("wrote %s (V=%lld, E=%lld)\n", name.str().c_str(), V, E);
    }

    return 0;
}
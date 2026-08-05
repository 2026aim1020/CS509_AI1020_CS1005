#include "csr.h"
#include "graph_algorithms.h"
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>

// Prints a distance value,INF for unreachable vertices.
static void print_distance(std::ostream &out, double d)
{
    if (std::isinf(d))
    {
        out << "INF";
        return;
    }
    double rounded = std::round(d);
    if (std::abs(d - rounded) < 1e-9)
    {
        out << static_cast<long long>(rounded);
    }
    else
    {
        out.precision(6);
        out << d;
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Error: missing input file.\n"
                  << "Usage: " << argv[0] << " <graph_input_file>\n";
        return 1;
    }

    std::string path = argv[1];
    AdjacencyList g;

    if (!read_adjacency_list(path, g))
    {
        std::cerr << "Error: could not read or parse input file '" << path << "'.\n";
        return 1;
    }
    if (!g.weighted)
    {
        std::cerr << "Warning: input has no edge weights; treating every edge as weight 1.\n";
    }

   
    CSR csr = build_csr(g);

    auto t1 = std::chrono::high_resolution_clock::now();
    SSSPResult result = sssp_dijkstra_csr(csr, g.source);
    auto t2 = std::chrono::high_resolution_clock::now();
    double sssp_ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

  
    std::cout << "Algorithm: SSSP\n";
    std::cout << "Source: " << g.source << "\n";
    std::cout << "Vertex Distance\n";
    for (int v = 0; v < csr.V; ++v)
    {
        std::cout << v << ' ';
        print_distance(std::cout, result.distance[v]);
        std::cout << "\n";
    }

    std::cout << "Execution time: " << sssp_ms << " ms\n";
    return 0;
}

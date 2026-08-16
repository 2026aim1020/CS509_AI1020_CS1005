// Driver for the Betweenness Centrality buddy task (Brandes' algorithm).
//
// Usage:
//   ./bc_driver <graph_input_file>

#include "../../assignment_01/include/csr.h"
#include "../include/graph_analytics.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

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

    CSR csr = build_csr(g); 

    auto t1 = std::chrono::high_resolution_clock::now();
    CentralityResult result = betweenness_centrality(csr);
    auto t2 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

    std::cout << "Algorithm: Betweenness Centrality\n";
    std::cout << "Vertex Centrality\n";
    std::cout << std::fixed << std::setprecision(2);
    for (int v = 0; v < csr.V; ++v)
    {
        std::cout << v << ' ' << result.centrality[v] << "\n";
    }
    std::cout.unsetf(std::ios::fixed);
    std::cout << "Execution time: " << ms << " ms\n";
    return 0;
}

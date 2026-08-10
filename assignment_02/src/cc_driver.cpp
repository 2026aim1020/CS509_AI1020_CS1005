#include "../../assignment_01/include/csr.h"
#include "../include/graph_analytics.h"
#include <chrono>
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
    ComponentsResult result = connected_components(csr);
    auto t2 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();
    std::cout << "Algorithm: Connected Components\n";
    std::cout << "Number of components: " << result.num_components << "\n";
    std::cout << "Vertex Component\n";
    for (int v = 0; v < csr.V; ++v)
    {
        std::cout << v << ' ' << result.component_id[v] << "\n";
    }
    std::cout << "Execution time: " << ms << " ms\n";
    return 0;
}

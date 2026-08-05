// Driver for the DFS buddy task.
#include "csr.h"
#include "graph_algorithms.h"
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

    // --- file reading ---
    if (!read_adjacency_list(path, g))
    {
        std::cerr << "Error: could not read or parse input file '" << path << "'.\n";
        return 1;
    }
    CSR csr = build_csr(g);

    // --- DFS ---
    auto t1 = std::chrono::high_resolution_clock::now();
    std::vector<int> traversal = dfs_csr(csr, g.source);
    auto t2 = std::chrono::high_resolution_clock::now();
    double dfs_ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

    // --- output ---
    std::cout << "Algorithm: DFS\n";
    std::cout << "Source: " << g.source << "\n";

    std::cout << "Traversal:";
    for (int v : traversal)
        std::cout << ' ' << v;
    std::cout << "\n";

    std::cout << "Execution time: " << dfs_ms << " ms\n";
    return 0;
}
